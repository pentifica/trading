#include "DivergeMonitor.h"

namespace pentifica::trd::exch {
//  ---------------------------------------------------------------------------
//
DivergeMonitor::DivergeMonitor(int threshold)
    : threshold_(threshold)
{
}
//  ---------------------------------------------------------------------------
//
void
DivergeMonitor::Monitor(std::string const& stock_one, std::string const& stock_two) {
    //  stock locator
    auto lookup = [&](std::string const& name) -> MonitoredStock& {
        auto entry = stocks_.find(name);
        if(entry != stocks_.end()) {
            return entry->second;
        }
        auto stock = std::make_shared<Stock>(name, 0);
        return stocks_.emplace(name, MonitoredStock(stock, {})).first->second;
    };

    //  update monitoring for stock one
    auto monitor_one = lookup(stock_one);
    auto monitor_two = lookup(stock_two);

    for(auto one : monitor_one.second) {
        for(auto two : monitor_two.second) {
            //  already monitored
            if(one == two) return;
        }
    }

    auto pair = std::make_shared<StockPair>(monitor_one.first, monitor_two.first);
    monitor_one.second.push_back(pair);
    monitor_two.second.push_back(pair);
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
    stock->second.first->price_ = price;

    //  check for divergence
    for(auto pair : stock->second.second) {
        if(pair->Diverged(threshold_)) {
        }
    }
}
//  ---------------------------------------------------------------------------
//
void
DivergeMonitor::Notify(Stock const& stock_one, Stock const& stock_two) {

}
//  ---------------------------------------------------------------------------
//
void
DivergeMonitor::Notify(std::string const& name, StockPair const& pair) {
    if(name == pair.StockOne().name_) {
        Notify(pair.StockOne(), pair.StockTwo());
    }

    else {
        Notify(pair.StockTwo(), pair.StockOne());
    }
}
//  ---------------------------------------------------------------------------
//
DivergeMonitor::StockPair::StockPair(StockPtr stock_one, StockPtr stock_two)
    : stock_one_(std::move(stock_one))
    , stock_two_(std::move(stock_two))
{
}
//  ---------------------------------------------------------------------------
//
bool
DivergeMonitor::StockPair::Diverged(int const threshold) {
    auto const diff = std::abs(stock_one_->price_ - stock_two_->price_);
    return diff > threshold;
}
}