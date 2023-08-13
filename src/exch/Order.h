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
#include    <chrono>
#include    <cstddef>
#include    <string>

namespace pentifica::trd::exch {
/// @brief Identifies the type of the order
enum class OrderSide:char {BUY = 'B', SELL = 'S', UNKNOWN = 'U'};
enum class OrderType:char {MARKET = 'M', LIMIT = 'L', UNKNOWN = 'U'};
enum class OrderTimeInForce:char {IOC = 'I', DAY = 'D', GTC = 'G', UNKNOWN = 'U'};
/// @brief  Defines the minimum information to describe a generic order
/// @tparam T   Specifies the data type of the price associated with the order.
template<typename T>
class Order {
public:
    using PriceType = T;
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    Order() = default;
    /// @brief  Specific initialization
    /// @param side 
    /// @param price 
    /// @param quantity 
    /// @param id 
    /// @param time 
    explicit Order(OrderSide side, OrderType type, OrderTimeInForce tif, T price,
        std::size_t quantity, std::string id, TimePoint time = Clock::now()) :
        price_{price},
        quantity_{quantity},
        time_{time},
        id_{std::move(id)},
        side_{side},
        type_{type},
        tif_{tif} {}
    Order(Order const&) = default;
    Order(Order&&) = default;
    Order& operator=(Order const&) = default;
    Order& operator=(Order&&) = default;

    void Price(T price) { price_ = price; }
    void Quantity(size_t quantity) { quantity_ = quantity; }
    void Time(TimePoint time) { time_ = time; }
    void Side(OrderSide side) { side_ = side; }
    void Type(OrderType type) { type_ = type; }
    void TIF(OrderTimeInForce tif) { tif_ = tif; }

    auto const& Id() const { return id_; }
    auto Price() const { return price_; }
    auto Quantity() const { return quantity_; }
    auto Time() const { return time_; }
    auto Side() const { return side_; }
    auto Type() const { return type_; }
    auto TIF() const { return tif_; }

private:
    T price_{};
    std::size_t quantity_{};
    TimePoint time_{};
    std::string id_;
    OrderSide side_{OrderSide::UNKNOWN};
    OrderType type_{OrderType::UNKNOWN};
    OrderTimeInForce tif_{OrderTimeInForce::UNKNOWN};
};
}