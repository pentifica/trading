#pragma once
/// @copyright {2023, Russell J. Fleming. All rights reserved.}
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
#include    <Order.h>

#include    <list>
#include    <map>
#include    <unordered_map>
#include    <memory>
#include    <exception>
#include    <algorithm>
#include    <functional>

#include    <iostream>
namespace pentifica::trd::exch {

/// @brief  Encapsulate information related to a trade signal
/// @tparam OrderDef Order definition
template<typename OrderDef>
struct EngineOnTrade {
    using OrderRef = std::shared_ptr<OrderDef>;
    OrderRef new_order_;
    OrderRef existing_order_;
    std::size_t quantity_;
};
/// @brief Encapsulate information related to a cancel signal
/// @tparam OrderDef Order definition
template<typename OrderDef>
struct EngineOnCancel {
    using OrderRef = std::shared_ptr<OrderDef>;
    OrderRef order_;
};
/// @brief Encapsualte information relted to a revise signal
/// @tparam OrderDef Order definition
template<typename OrderDef>
struct EngineOnRevise {
    using OrderRef = std::shared_ptr<OrderDef>;
    OrderRef order_;
};
/// @brief A simple matching engine that matches orders by price then time.
/// @tparam OrderDef The order type
/// @tparam Callback Provides handling for callback notification. Callbacks are
///                  based on operator() overloading with a single discriminator
///                  argument. Currently defined discriminator arguments are:
///                     - EngineOnTrade
///                     - EngineOnCancel
///                     - EngineOnRevise
template<typename OrderDef, typename Callback>
class MatchingEngine {
public:
    using OrderRef = std::shared_ptr<OrderDef>;
    using PriceType = OrderDef::PriceType;
    using PriceRung = std::list<OrderRef>;
    using BuyLadder = std::map<PriceType, PriceRung, std::greater<PriceType>>;
    using SellLadder = std::map<PriceType, PriceRung>;
    using OrderBook = std::unordered_map<std::string, OrderRef>;
    using OnTrade = EngineOnTrade<OrderDef>;
    using OnCancel = EngineOnCancel<OrderDef>;
    using OnRevise = EngineOnRevise<OrderDef>;

    explicit MatchingEngine(Callback callback) : callback_(callback) {}
    MatchingEngine(MatchingEngine const&) = delete;
    MatchingEngine(MatchingEngine&&) = delete;
    ~MatchingEngine() = default;
    MatchingEngine& operator=(MatchingEngine const&) = delete;
    MatchingEngine& operator=(MatchingEngine&&) = delete;

    void Buy(OrderRef& order) { Fill(order, sell_ladder_, buy_ladder_); }
    void Sell(OrderRef& order) { Fill(order, buy_ladder_, sell_ladder_); }
    /// @brief Cancel an order from the book
    /// @param id The order identifier
    void Cancel(std::string const& id) {
        auto index{order_book_.find(id)};
        if(index == order_book_.end()) return;
        auto& [_, order] = *index;

        OnCancel response{order};
        LadderDel(order);
        order_book_.erase(index);
        callback_(response);
    }
    /// @brief Revise an order's chacteristics
    /// @param order The order's new characteristics
    void Revise(OrderRef& order) {
        auto index{order_book_.find(order->Id())};
        if(index == order_book_.end()) return;
        
        auto& [_, original_order] = *index;
        LadderDel(original_order);
        order_book_.erase(index);

        switch(order->Side()) {
            case OrderSide::BUY:    Fill(order, sell_ladder_, buy_ladder_); break;
            case OrderSide::SELL:   Fill(order, buy_ladder_, sell_ladder_); break;
            default: throw std::logic_error("Order side invalid");
        }
        callback_(OnRevise{order});
    }

private:
    /// @brief Removes an order from a buy/sell ladder
    /// @param order The order to remove
    void LadderDel(OrderRef& order) {
        switch(order->Side()) {
            case OrderSide::BUY:    LadderDel(order, buy_ladder_);  return;
            case OrderSide::SELL:   LadderDel(order, sell_ladder_); return;
            default:    throw std::logic_error("Order side invalid");
        }
    }
    /// @brief Removes an order from the indicated ladder
    /// @tparam Ladder Type of ladder (buy/sell)
    /// @param order The order to remove
    /// @param ladder The ladder to update
    template<typename Ladder>
    void LadderDel(OrderRef& order, Ladder& ladder) {
        auto& rung = ladder[order->Price()];
        auto index = std::find_if(rung.begin(), rung.end(),
            [&id = order->Id()](auto&& index) noexcept {
                return id == index->Id();
            });
        if(index != rung.end()) rung.erase(index);
    }
    /// @brief RAII helper based on a funtion
    /// @tparam Action The function type
    template<typename Action>
    struct RAII {
        explicit RAII(Action action) : action_(action) {}
        ~RAII() { action_(); }
    private:
        Action action_;
    };
    /// @brief Fill an order from existing orders
    /// @tparam Compare Price ladder containing opposing orders
    /// @tparam Store Price ladder to store the order in if not fully filled
    /// @param order The order to fill
    /// @param compare The price ladder to fill the order from
    /// @param store Where to store the unfilled order
    template<typename Compare, typename Store>
    void Fill(OrderRef& order, Compare& compare, Store& store) {

        auto wrapup = [this, &order, &ladder=store]() {
            if((order->Type() == OrderType::MARKET)
                || (order->TIF() == OrderTimeInForce::IOC)
                || (order->Quantity() == 0)) return;
            ladder[order->Price()].push_back(order);
            order_book_[order->Id()] = order;
        };

        RAII finally{wrapup};

        if(compare.empty()) return;

        auto remaining{order->Quantity()};
        auto target_price{order->Price()};
        if(order->Type() == OrderType::MARKET) {
            target_price = compare.rbegin()->first;
        }

        for(auto& [price, rung] : compare) {
            if(remaining == 0) break;
            if(compare.key_comp()(target_price, price)) break;

            while(remaining && !rung.empty()) {
                auto& rung_order{rung.front()};
                auto quantity{rung_order->Quantity()};
                auto const matched{std::min(remaining, quantity)};

                remaining -= matched;
                quantity -= matched;

                order->Quantity(remaining);
                rung_order->Quantity(quantity);

                callback_(OnTrade{order, rung_order, matched});

                if(quantity == 0) {
                    order_book_.erase(rung_order->Id());
                    rung.pop_front();
                }
            }
        }
    }

private:
    OrderBook order_book_{};
    BuyLadder buy_ladder_{};
    SellLadder sell_ladder_{};
    Callback callback_;
};
}