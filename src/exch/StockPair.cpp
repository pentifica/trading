#include "StockPair.h"

namespace pentifica::trd::exch {
//  ---------------------------------------------------------------------------
//
StockPair::StockPair(StockPtr stock_one, StockPtr stock_two)
    : stock_one_(std::move(stock_one))
    , stock_two_(std::move(stock_two))
{
}
//  ---------------------------------------------------------------------------
//
bool
StockPair::Diverged(int const threshold) {
    if(stock_one_->price_ == 0 || stock_two_->price_ == 0)
        return false;

    auto const diff = std::abs(stock_one_->price_ - stock_two_->price_);
    return diff > threshold;
}
}