#pragma once

#include "Stock.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <list>

namespace pentifica::trd::exch {

/// @brief A stock threshold divergence monitor
class DivergeMonitor {
public:
    /// @brief Initialize a stock monitor
    /// @param threshold 
    explicit DivergeMonitor(int threshold);
    /// @brief Start monitoring the pair of stocks identified
    /// @param stock_one The first stock to monitor
    /// @param stock_two The second stock to monitor
    void Monitor(std::string const& stock_one, std::string const& stock_two);
    /// @brief Update the price of the identified stock
    /// @param name 
    /// @param price 
    void Update(std::string const& name, int price);
    /// @brief Identifies a stock pair to be monitored
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
        Stock const& StockOne() const { return *stock_one_; }
        Stock const& StockTwo() const { return *stock_two_; }

    private:
        StockPtr stock_one_;
        StockPtr stock_two_;
    };

private:
    /// @brief Send a notification fo the stocks that have diverged
    /// @param stock_one 
    /// @param stock_two 
    void Notify(Stock const& stock_one, Stock const& stock_two);
    void Notify(std::string const& name, StockPair const& pair);

    /// @brief The threshold for stock divergence
    int const threshold_;
    using StockPtr = std::shared_ptr<Stock>;
    using PairPtr = std::shared_ptr<StockPair>;
    using MonitoredStock = std::pair<StockPtr, std::list<PairPtr>>;
    /// @brief Lookup for monitored stocks
    std::unordered_map<std::string, MonitoredStock> stocks_;
};
}