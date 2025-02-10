#pragma once

#include <string>

namespace pentifica::trd::exch {
/// @brief POD for stock traits
struct Stock {
    /// @brief The name of the stock
    std::string name_;
    /// @brief The current stock price
    int price_;
};
}
