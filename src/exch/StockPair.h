#pragma once

#include "Stock.h"

#include <memory>

namespace pentifica::trd::exch {
/// @brief Represents a coupled pair of stocks
class StockPair {
public:
    using StockPtr = std::shared_ptr<Stock>;
    /// @brief Prepare am instance
    /// @param stock_one 
    /// @param stock_two 
    explicit StockPair(StockPtr stock_one, StockPtr stock_two);
    /// @brief Indicates if the stock prices have diverged
    /// @param threshold Threshold for divergence
    /// @return 
    bool Diverged(int const threshold);
    /// @brief Provides the first stock in the pair
    /// @return The first stock in the pair
    Stock const& StockOne() const { return *stock_one_; }
    /// @brief Provides the second stock in the pair
    /// @return The second stock in the pair
    Stock const& StockTwo() const { return *stock_two_; }

private:
    /// @brief The first stock in the pair
    StockPtr stock_one_;
    /// @brief The second stock in the pair
    StockPtr stock_two_;
};
}

inline
std::ostream&
operator<<(std::ostream& os, pentifica::trd::exch::StockPair const& sp) {
    os << "one[" << sp.StockOne() << "] two[" << sp.StockTwo() << ']';
    return os;
}