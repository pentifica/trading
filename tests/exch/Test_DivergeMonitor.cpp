#include <DivergeMonitor.h>


#include <gtest/gtest.h>

TEST(Test_DivergeMonitor, Initialization) {
    using namespace pentifica::trd::exch;

    constexpr int threshold{5};
    DivergeMonitor monitor(threshold);
}