#include <DivergeMonitor.h>


#include <gtest/gtest.h>

#include <sstream>

//  ---------------------------------------------------------------------------
//
TEST(Test_Stock, initialization) {
    using namespace pentifica::trd::exch;
    constexpr char expected[] = "name[test] price[5]";
    
    Stock stock{"test", 5};
    std::ostringstream oss;
    oss << stock;

    EXPECT_STREQ(expected, oss.str().c_str());
}

//  ---------------------------------------------------------------------------
//
TEST(Test_StockPair, initialization) {
    using namespace pentifica::trd::exch;
    constexpr char expected[] = "one[name[a] price[1]] two[name[b] price[2]]";

    auto a = std::make_shared<Stock>("a", 1);
    auto b = std::make_shared<Stock>("b", 2);

    StockPair pair(a, b);

    std::ostringstream oss;
    oss << pair;

    EXPECT_STREQ(expected, oss.str().c_str());
}

//  ---------------------------------------------------------------------------
//
TEST(Test_DivergeMonitor, initialization) {
    using namespace pentifica::trd::exch;

    constexpr int threshold{5};
    auto on_notify = [](Stock const&, Stock const&) {};
    DivergeMonitor monitor(threshold, on_notify);
}

//  ---------------------------------------------------------------------------
//
TEST(Test_DivergeMonitor, bad_initialization) {
    using namespace pentifica::trd::exch;

    constexpr int threshold{0};
    auto on_notify = [](Stock const&, Stock const&) {};
    try {
        DivergeMonitor monitor(threshold, on_notify);
        FAIL() << "Exception not reported";
    }

    catch(std::invalid_argument& e) {
        ASSERT_STREQ("threshold <= 0", e.what());
    }

    catch(...) {
        FAIL() << "Unknown exception received";
    }
}

//  ----------------------------------------------------------------------------
//
TEST(Test_DivergeMonitor, monitor) {
    using namespace pentifica::trd::exch;

    constexpr int threshold{5};
    auto on_notify = [](Stock const&, Stock const&) {};

    DivergeMonitor monitor(threshold, on_notify);

    struct {
        std::string one_;
        std::string two_;
        std::string expected_;
    } tests[] = {
        {"a", "b", "one[name[a] price[0]] two[name[b] price[0]]"},
        {"c", "d", "one[name[c] price[0]] two[name[d] price[0]]"},
        {"e", "f", "one[name[e] price[0]] two[name[f] price[0]]"},
        {"g", "h", "one[name[g] price[0]] two[name[h] price[0]]"},
        {"i", "j", "one[name[i] price[0]] two[name[j] price[0]]"},
    };

    auto check_pairing = [&](std::string const& name, std::string const& expected) {
        auto const& pairing = monitor.GetStockPairings(name);
        std::ostringstream oss;
        oss << *pairing.front();
        EXPECT_STREQ(oss.str().c_str(), expected.c_str());
    };

    for(auto const& test : tests) {
        monitor.Monitor(test.one_, test.two_);
        check_pairing(test.one_, test.expected_);
        check_pairing(test.two_, test.expected_);
    }
}

//  ----------------------------------------------------------------------------
//
TEST(Test_DivergeMonitor, return_pairings) {
    using namespace pentifica::trd::exch;

    constexpr int threshold{5};
    auto on_notify = [](Stock const&, Stock const&) {};

    DivergeMonitor monitor(threshold, on_notify);

    struct {
        std::string name_;
        using TestData = struct {
            std::string one_;
            std::string two_;
            std::string expected_;
        };
        std::vector<TestData> tests_;
    } tests[] = {
        {
            "a",
            {
                {"a", "b", "one[name[a] price[0]] two[name[b] price[0]]"},
                {"a", "c", "one[name[a] price[0]] two[name[c] price[0]]"},
                {"a", "d", "one[name[a] price[0]] two[name[d] price[0]]"},
                {"a", "e", "one[name[a] price[0]] two[name[e] price[0]]"},
                {"a", "f", "one[name[a] price[0]] two[name[f] price[0]]"},
            }
        },
        {
            "g",
            {
                {"g", "h", "one[name[g] price[0]] two[name[h] price[0]]"},
                {"g", "i", "one[name[g] price[0]] two[name[i] price[0]]"},
                {"g", "j", "one[name[g] price[0]] two[name[j] price[0]]"},
                {"g", "k", "one[name[g] price[0]] two[name[k] price[0]]"},
                {"g", "l", "one[name[g] price[0]] two[name[l] price[0]]"},
            }
        }
    };

    auto check_pairing = [&](std::string const& name, auto const& tests) {
        std::set<std::string> expected;
        for(auto const& [one, two, result] : tests) {
            monitor.Monitor(one, two);
            expected.emplace(result);
        }
        for(auto const& pairing : monitor.GetStockPairings(name)) {
            std::ostringstream oss;
            oss << *pairing;
            EXPECT_TRUE(expected.find(oss.str()) != expected.end())
                << "could not find: " << oss.str();
        }
    };

    for(auto const& [name, test] : tests) {
        check_pairing(name, test);
    }
}

TEST(Test_DivergeMonitor, update) {
    using namespace pentifica::trd::exch;

    constexpr int threshold{5};
    bool notified{false};
    std::string stock_one{};
    std::set<std::string> stock_two{};

    auto test_reset = [&]() {
        notified = false;
        stock_one.clear();
        stock_two.clear();
    };

    auto on_notify = [&](Stock const& one, Stock const& two) {
        notified = true;
        stock_one = one.name_;
        stock_two.insert(two.name_);
    };

    DivergeMonitor monitor(threshold, on_notify);

    //  create stock pairings
    struct {
        std::string one_;
        std::string two_;
    }   pairings[] = {
        { "a", "b" }, { "a", "c" }, { "a", "d" },
        { "b", "c" }, { "b", "d" },
        { "c", "d" }
    };
    for(auto const& [one, two] : pairings) {
        monitor.Monitor(one, two);
    }

    //  test notifications
    struct {
        std::string name_;
        int price_;
        bool notify_;
        std::vector<std::string> diverged_;
    }   tests[] = {
        { "a", 1,   false, {} },                //  a = 1   b = 0   c = 0   d = 0
        { "b", 7,   true,  {"a"}},              //  a = 1   b = 7   c = 0   d = 0
        { "c", 13,  true,  {"a", "b"} },        //  a = 1   b = 7   c = 13  d = 0
        { "d", 7,   true,  {"a", "c"} },        //  a = 1   b = 7   c = 13  d = 7
        { "a", 2,   true,  {"c"} },             //  a = 2   b = 7   c = 13  d = 7
        { "a", 1,   true,  {"b", "c", "d"} },   //  a = 1   b = 7   c = 13  d = 7
    };

    for(auto const& [name_, price_, notify_, diverged_] : tests) {
        test_reset();
        monitor.Update(name_, price_);
        EXPECT_EQ(notify_, notified);
        if(notified) {
            EXPECT_STREQ(name_.c_str(), stock_one.c_str());
            EXPECT_EQ(diverged_.size(), stock_two.size());
            for(auto const& name : diverged_) {
                EXPECT_TRUE(stock_two.find(name) != stock_two.end())
                    << "Cannot find " << name << " in list of reported stocks";
            }
        }
    }
}