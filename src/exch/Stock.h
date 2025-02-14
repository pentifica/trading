#pragma once

#include <string>
#include <iostream>

namespace pentifica::trd::exch {
/// @brief POD for stock traits
struct Stock {
    /// @brief The name of the stock
    std::string name_;
    /// @brief The current stock price
    int price_;
};
}

inline
std::ostream&
operator<<(std::ostream& os, pentifica::trd::exch::Stock const& stock) {
    os << "name[" << stock.name_ << "] price[" << stock.price_ << ']';
    return os;
}