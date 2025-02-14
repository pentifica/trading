#include "DivergeMonitor.h"

#include <exception>

namespace pentifica::trd::exch {
//  ---------------------------------------------------------------------------
//
DivergeMonitor::DivergeMonitor(int threshold, OnNotify on_notify)
    : threshold_(threshold)
    , on_notify_(std::move(on_notify))
{
    if(threshold_ <= 0) {
        throw std::invalid_argument("threshold <= 0");
    }
}
//  ---------------------------------------------------------------------------
//
void
DivergeMonitor::Monitor(std::string const& stock_one, std::string const& stock_two) {
    //  check that stocks are different
    if(stock_one == stock_two) {
        return;
    }

    //  stock locator
    auto locate = [&](std::string const& name) -> MonitoredStock& {
        auto entry = stocks_.find(name);
        if(entry != stocks_.end()) {
            return entry->second;
        }
        auto stock = std::make_shared<Stock>(name, 0);
        stocks_[name] = MonitoredStock(stock, {});
        return stocks_[name];
    };

    //  check if monitoring already in place
    auto& [stock_1, list_1] = locate(stock_one);
    auto& [stock_2, list_2] = locate(stock_two);

    for(auto const& one : list_1) {
        for(auto const& two : list_2) {
            //  already monitored
            if(one == two) return;
        }
    }

    //  start monitoring   
    auto stock_pair = std::make_shared<StockPair>(stock_1, stock_2);
    list_1.push_back(stock_pair);
    list_2.push_back(stock_pair);
}
//  ---------------------------------------------------------------------------
//
void
DivergeMonitor::Update(std::string const& name, int price) {
    //  lookup monitored stock
    auto stock = stocks_.find(name);

    //  if not already monitored, just add
    if(stock == stocks_.end()) {
        StockPtr new_stock = std::make_shared<Stock>(name, price);
        stocks_.emplace(name, MonitoredStock(new_stock, {}));
        return;
    }

    //  update stock price
    auto& [_, entry] = *stock;
    auto& [traits, pairings] = entry;
    traits->price_ = price;

    //  check for divergence
    for(auto stock_pair : pairings) {
        if(stock_pair->Diverged(threshold_)) {
            Notify(name, *stock_pair);
        }
    }
}
//  ---------------------------------------------------------------------------
//
DivergeMonitor::StockPairings const&
DivergeMonitor::GetStockPairings(std::string const& name) const {
    static  StockPairings const empty;
    auto entry = stocks_.find(name);
    if(entry != stocks_.end()) {
        return entry->second.second;
    }

    return empty;
}
//  ---------------------------------------------------------------------------
//
void
DivergeMonitor::Notify(std::string const& name, StockPair const& stock_pair) {
    if(name == stock_pair.StockOne().name_) {
        on_notify_(stock_pair.StockOne(), stock_pair.StockTwo());
    }

    else {
        on_notify_(stock_pair.StockTwo(), stock_pair.StockOne());
    }
}
}