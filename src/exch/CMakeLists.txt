target_sources(trading
    PRIVATE
        DivergeMonitor.h
        DivergeMonitor.cpp
        Order.h
        Stock.h
        StockPair.h
        StockPair.cpp
)

set_target_properties(trading
    PROPERTIES LINKER_LANGUAGE CXX
)