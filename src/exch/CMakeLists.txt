target_sources(trading
    PRIVATE
        DivergeMonitor.h
        DivergeMonitor.cpp
        Order.h
        Stock.h
)

set_target_properties(trading
    PROPERTIES LINKER_LANGUAGE CXX
)