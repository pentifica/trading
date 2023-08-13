#include    <Order.h>

#include    <gtest/gtest.h>

TEST(Test_Order, DefaultCtor) {
    using namespace pentifica::trd::exch;
    using TestOrder = Order<int>;

    TestOrder order;
    EXPECT_EQ(order.Price(), 0);
    EXPECT_EQ(order.Quantity(), 0);
    EXPECT_EQ(order.Side(), OrderSide::UNKNOWN);
    EXPECT_EQ(order.Type(), OrderType::UNKNOWN);
    EXPECT_EQ(order.TIF(), OrderTimeInForce::UNKNOWN);
    EXPECT_TRUE(order.Id().empty());
}

TEST(Test_Order, Initialization) {
    using namespace pentifica::trd::exch;
    using TestOrder = Order<int>;

    constexpr int expected_price{20};
    constexpr std::size_t expected_quantity{300};
    constexpr auto expected_side{OrderSide::BUY};
    constexpr auto expected_type{OrderType::MARKET};
    constexpr auto expected_tif{OrderTimeInForce::DAY};
    TestOrder::TimePoint  const expected_time{TestOrder::Clock::now()};
    std::string  const expected_id{"this is a test"};

    TestOrder order(expected_side,
                    expected_type,
                    expected_tif,
                    expected_price,
                    expected_quantity,
                    expected_id,
                    expected_time);

    EXPECT_EQ(order.Price(), expected_price);
    EXPECT_EQ(order.Quantity(), expected_quantity);
    EXPECT_EQ(order.Side(), expected_side);
    EXPECT_EQ(order.Type(), expected_type);
    EXPECT_EQ(order.TIF(), expected_tif);
    EXPECT_EQ(order.Time(), expected_time);
    EXPECT_TRUE(order.Id() == expected_id);
}

TEST(Test_Order, Modify) {
    using namespace pentifica::trd::exch;
    using TestOrder = Order<int>;

    constexpr int initial_price{20};
    constexpr std::size_t initial_quantity{300};
    constexpr auto initial_side{OrderSide::BUY};
    constexpr auto initial_type{OrderType::MARKET};
    constexpr auto initial_tif{OrderTimeInForce::DAY};
    TestOrder::TimePoint  const initial_time{TestOrder::Clock::now()};
    std::string  const initial_id{"this is a test"};

    TestOrder order(initial_side,
                    initial_type,
                    initial_tif,
                    initial_price,
                    initial_quantity,
                    initial_id,
                    initial_time);


    constexpr int expected_price{30};
    constexpr std::size_t expected_quantity{100};
    constexpr auto expected_side{OrderSide::SELL};
    constexpr auto expected_type{OrderType::LIMIT};
    constexpr auto expected_tif{OrderTimeInForce::GTC};
    TestOrder::TimePoint  const expected_time{TestOrder::Clock::now()};

    EXPECT_NE(order.Price(), expected_price);
    EXPECT_NE(order.Quantity(), expected_quantity);
    EXPECT_NE(order.Side(), expected_side);
    EXPECT_NE(order.Type(), expected_type);
    EXPECT_NE(order.TIF(), expected_tif);
    EXPECT_NE(order.Time(), expected_time);

    order.Price(expected_price);
    EXPECT_EQ(order.Price(), expected_price);

    order.Quantity(expected_quantity);
    EXPECT_EQ(order.Quantity(), expected_quantity);

    order.Side(expected_side);
    EXPECT_EQ(order.Side(), expected_side);

    order.Type(expected_type);
    EXPECT_EQ(order.Type(), expected_type);

    order.TIF(expected_tif);
    EXPECT_EQ(order.TIF(), expected_tif);

    order.Time(expected_time);
    EXPECT_EQ(order.Time(), expected_time);
}