#include    <Order.h>
#include    <MatchingEngine.h>

#include    <gtest/gtest.h>

#include    <iostream>
#include    <unordered_map>
#include    <unordered_set>
#include    <string>
#include    <random>

template<typename ...Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};
//template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

namespace {
    using namespace pentifica::trd::exch;

    using TestOrder = Order<int>;
    using OnTrade = EngineOnTrade<TestOrder>;
    using OnCancel = EngineOnCancel<TestOrder>;
    using OnRevise = EngineOnRevise<TestOrder>;

    struct OrderSpec {
        std::string id_;
        int price_;
        std::size_t quantity_;
        OrderSide side_;
    };
}

TEST(Test_MatchingEngine, Initialization) {
    using namespace pentifica::trd::exch;
    auto callback = Overload {
        [](auto) { std::cout << "Unhandled matching engine callback\n"; }
    };

    using Callback = decltype(callback);

    MatchingEngine<TestOrder, Callback> engine(callback);
}

namespace {
    OrderSpec buy_orders[] = {
        { "First   ", 1, 10, OrderSide::BUY },
        { "Second  ", 2, 10, OrderSide::BUY },
        { "Third   ", 3, 10, OrderSide::BUY },
        { "Fourth  ", 4, 10, OrderSide::BUY },
        { "Fifth   ", 5, 10, OrderSide::BUY },
        { "Sixth   ", 6, 10, OrderSide::BUY },
        { "Seventh ", 7, 10, OrderSide::BUY },
        { "Eighth  ", 8, 10, OrderSide::BUY },
        { "Nineth  ", 9, 10, OrderSide::BUY },
    };
}

TEST(Test_MatchingEngine, BuyCancel) {
    using namespace pentifica::trd::exch;

    std::unordered_map<std::string, int> cancelled;
    auto callback = Overload {
        [&cancelled](OnCancel& info) { cancelled[info.order_->Id()]++; },
        [](auto) { std::cerr << "Unhandled matching engine callback\n"; }
    };

    using Callback = decltype(callback);

    MatchingEngine<TestOrder, Callback> engine(callback);

    for(auto const& [id, price, quantity, side] : buy_orders) {
        constexpr auto type = OrderType::LIMIT;
        constexpr auto tif = OrderTimeInForce::DAY;
        auto order = std::make_shared<TestOrder>(side, type, tif, price, quantity, id);
        engine.Buy(order);
    }

    EXPECT_TRUE(cancelled.empty());

    for(auto const& [id, _1, _2, _3] : buy_orders) {
        engine.Cancel(id);
    }

    EXPECT_EQ(cancelled.size(), sizeof(buy_orders)/sizeof(buy_orders[0]));

    for(auto const& [id, _1, _2, _3] : buy_orders) {
        EXPECT_EQ(cancelled[id], 1);
    }
}

namespace {
    OrderSpec sell_orders[] = {
        { "First   ", 1, 10, OrderSide::SELL },
        { "Second  ", 2, 10, OrderSide::SELL },
        { "Third   ", 3, 10, OrderSide::SELL },
        { "Fourth  ", 4, 10, OrderSide::SELL },
        { "Fifth   ", 5, 10, OrderSide::SELL },
        { "Sixth   ", 6, 10, OrderSide::SELL },
        { "Seventh ", 7, 10, OrderSide::SELL },
        { "Eighth  ", 8, 10, OrderSide::SELL },
        { "Nineth  ", 9, 10, OrderSide::SELL },
    };
}

TEST(Test_MatchingEngine, SellCancel) {
    using namespace pentifica::trd::exch;

    std::unordered_map<std::string, int> cancelled;
    auto callback = Overload {
        [&cancelled](OnCancel& info) { cancelled[info.order_->Id()]++; },
        [](auto) { std::cerr << "Unhandled matching engine callback\n"; }
    };

    using Callback = decltype(callback);

    MatchingEngine<TestOrder, Callback> engine(callback);

    for(auto const& [id, price, quantity, side] : sell_orders) {
        constexpr auto type = OrderType::LIMIT;
        constexpr auto tif = OrderTimeInForce::DAY;
        auto order = std::make_shared<TestOrder>(side, type, tif, price, quantity, id);
        engine.Buy(order);
    }

    EXPECT_TRUE(cancelled.empty());

    for(auto const& [id, _1, _2, _3] : sell_orders) {
        engine.Cancel(id);
    }

    EXPECT_EQ(cancelled.size(), sizeof(sell_orders)/sizeof(sell_orders[0]));

    for(auto const& [id, _1, _2, _3] : sell_orders) {
        EXPECT_EQ(cancelled[id], 1);
    }
}

namespace {
    static char const alphanumeric[] = "0123456789"
                                       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                       "abcdefghijklmnopqrstuvwxyz";
    constexpr auto alphanumeric_length = sizeof(alphanumeric) - 1;


    constexpr std::size_t id_length{20};

    std::string GenerateId() {
        static std::random_device dev;
        static std::uniform_int_distribution<std::mt19937::result_type> dist(0, alphanumeric_length - 1);
        static std::mt19937 rng(dev());

        std::string name;
        name.reserve(id_length);
        for(std::size_t index = 0; index < id_length; ++index) {
            name += alphanumeric[dist(rng)];
        }
        return name;
    }

    int GeneratePrice() {
        static std::random_device dev;
        static std::uniform_int_distribution<std::mt19937::result_type> dist(100, 1000);
        static std::mt19937 rng(dev());

        return dist(rng);
    }

    std::size_t GenerateQuantity() {
        static std::random_device dev;
        static std::uniform_int_distribution<std::mt19937::result_type> dist(10000, 100000);
        static std::mt19937 rng(dev());

        return dist(rng);
    }

    std::unordered_set<std::string> GenerateIds(std::size_t count) {
        std::unordered_set<std::string> result;
        for(std::size_t total = 0; total < count; ++total) {
            for(;;) {
                auto [_, added] = result.emplace(GenerateId());
                if(added) break;
            }
        }
        return result;
    }
}

TEST(Test_MatchingEngine, ReviseOrder) {
    using namespace pentifica::trd::exch;

    std::unordered_map<std::string, std::size_t> revise_count;

    auto callback = Overload {
        [&revise_count](OnRevise const& info) {
            revise_count[info.order_->Id()]++;
        },
        [](auto) { std::cerr << "Unhandled engine callback\n"; }
    };

    using Callback = decltype(callback);

    MatchingEngine<TestOrder, Callback> engine(callback);

    std::unordered_set<std::string> order_ids;
    constexpr std::size_t order_count{10};
    auto const& ids = GenerateIds(order_count);

    for(auto const& id : ids) {
        constexpr auto type = OrderType::LIMIT;
        constexpr auto tif = OrderTimeInForce::DAY;
        constexpr auto side = OrderSide::BUY;
        auto order = std::make_shared<TestOrder>(side, type, tif, GeneratePrice(),
            GenerateQuantity(), id);
        engine.Buy(order);
    }

    for(auto const& id : ids) {
        constexpr auto type = OrderType::LIMIT;
        constexpr auto tif = OrderTimeInForce::DAY;
        constexpr auto side = OrderSide::BUY;
        auto order = std::make_shared<TestOrder>(side, type, tif, GeneratePrice(),
            GenerateQuantity(), id);
        engine.Revise(order);
    }

    EXPECT_EQ(revise_count.size(), order_count);
    for(auto& [id, count] : revise_count) {
        EXPECT_EQ(count, 1);
    }
}

namespace {
    struct TradingActivity {
        OrderSide side_;
        int price_;
        std::size_t quantity_;
    };

}

TEST(Test_MatchingEngine, IOCTrade) {
    using namespace pentifica::trd::exch;

    std::size_t quantity_traded;
    std::string cancel_id;

    auto callback = Overload {
        [&quantity_traded](OnTrade const& info) { quantity_traded = info.quantity_; },
        [&cancel_id](OnCancel const& info) { cancel_id = info.order_->Id(); },
        [](auto) { std::cerr << "Unhandled matching engine callback\n"; }
    };

    using Callback = decltype(callback);

    MatchingEngine<TestOrder, Callback> engine(callback);

    struct Scenario {
        struct {
            int price_;
            std::size_t quantity_;
            std::size_t expected_;
        } sell_, buy_;
    };

    Scenario scenario[] = {
        { {100, 1000,  200}, {110, 800,   0} },
        { {100,  600,    0}, {110, 800, 200} },
        { {120, 1000, 1000}, {110, 800, 800} },
    };

    for(auto& [sell_data, buy_data] : scenario) {
        auto sell = std::make_shared<TestOrder>(OrderSide::SELL,
                                                OrderType::LIMIT,
                                                OrderTimeInForce::DAY,
                                                sell_data.price_,
                                                sell_data.quantity_,
                                                GenerateId());
        auto buy = std::make_shared<TestOrder>(OrderSide::BUY,
                                               OrderType::LIMIT,
                                               OrderTimeInForce::IOC,
                                               buy_data.price_,
                                               buy_data.quantity_,
                                               GenerateId());
        
        engine.Sell(sell);
        engine.Buy(buy);

        EXPECT_EQ(sell->Quantity(), sell_data.expected_);
        EXPECT_EQ(buy->Quantity(), buy_data.expected_);

        cancel_id.clear();
        engine.Cancel(sell->Id());
        if(sell->Quantity() != 0)   EXPECT_TRUE(cancel_id == sell->Id());
        else                        EXPECT_TRUE(cancel_id != sell->Id());

        engine.Cancel(buy->Id());
        EXPECT_FALSE(cancel_id == buy->Id());
    }
}