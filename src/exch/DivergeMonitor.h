#pragma once

#include "StockPair.h"

#include <unordered_map>
#include <list>
#include <functional>

namespace pentifica::trd::exch { class DivergeMonitor; }
/// @brief Stream state of @ref DivergeMonitor
/// @param  os  Where to stream the the state
/// @param  state   The state to stream
/// @return The stream provided
std::ostream& operator<<(std::ostream& os, pentifica::trd::exch::DivergeMonitor const& state);

namespace pentifica::trd::exch {

/// @brief A stock threshold divergence monitor
class DivergeMonitor {
public:
    //  friend(s)
    friend std::ostream& operator<<(std::ostream&, DivergeMonitor const&);
    //  definitions
    using OnNotify = std::function<void(Stock const& one, Stock const& two)>;
    using StockPtr = std::shared_ptr<Stock>;
    using PairPtr = std::shared_ptr<StockPair>;
    using StockPairings = std::list<PairPtr>;
    using MonitoredStock = std::pair<StockPtr, StockPairings>;
    /// @brief Initialize a stock monitor
    /// @param threshold 
    explicit DivergeMonitor(int threshold, OnNotify on_notify);
    /// @brief Start monitoring the pair of stocks identified
    /// @param stock_one The first stock to monitor
    /// @param stock_two The second stock to monitor
    void Monitor(std::string const& stock_one, std::string const& stock_two);
    /// @brief Update the price of the identified stock
    /// @param name 
    /// @param price 
    void Update(std::string const& name, int price);
    /// @brief Returns a list of stocks the names stock is paired with
    /// @param name The name of the stock to lookup
    /// @return A, possibly empty, list of pairings involving the named stock
    StockPairings const& GetStockPairings(std::string const& name) const;

private:
    /// @brief Notify of a threshold divergence
    /// @param name Name of the stock whose price changed
    /// @param pair The stock pair contianing this stock
    void Notify(std::string const& name, StockPair const& pair);
    /// @brief The threshold for stock divergence
    int const threshold_{};
    /// @brief Notifcation callback
    OnNotify on_notify_{};
    /// @brief Lookup for monitored stocks
    std::unordered_map<std::string, MonitoredStock> stocks_;
};
}