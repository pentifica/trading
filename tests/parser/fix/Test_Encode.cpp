#include    <parser/fix/Encode.h>

#include    <gtest/gtest.h>

#include    <array>
#include    <iostream>
#include    <string_view>
#include    <tuple>
#include    <utility>

namespace {
    template<typename T>
    void Dump(T&& begin, std::size_t length) {
        for(; length != 0; --length, begin++) {
            if(*begin == pentifica::trd::fix::SOH) std::cerr << '|';
            else std::cerr << *begin;
        }
        std::cerr << '\n';
    }
}

TEST(Test_Encode, wrapper) {
    using namespace pentifica::trd::fix;
    std::array<Byte, 256> buffer;

    Encode message(MsgType::LOGON, Version::_4_2, buffer.begin(), buffer.end());
    message.Finalize(789, 790);

    std::string_view message_view((char*) buffer.begin(), (char*) (buffer.begin() + message.Size()));

    constexpr std::size_t overhead{19 + 6};
    constexpr std::size_t body{57};
    EXPECT_EQ(overhead + body, static_cast<std::size_t>(message.Size()));

    EXPECT_EQ(message_view[0], '8');
    EXPECT_TRUE(message_view.find("=FIX.4.2"));
    EXPECT_TRUE(message_view.find("9=000057"));
    EXPECT_TRUE(message_view.find("35=A"));
    EXPECT_TRUE(message_view.find("34=000000789"));
    EXPECT_TRUE(message_view.find("369=000000790"));
    EXPECT_TRUE(message_view.find("10="));
}

namespace {
    using namespace pentifica::trd::fix;

    template<typename TupleType, std::size_t ...Is>
    void TestAppend(TupleType const& tp, std::index_sequence<Is...>) {
        auto field_test = [](auto const& test_data) {
            std::array<Byte, 256> buffer;
            Encode message(MsgType::LOGON, Version::_4_2, buffer.begin(), buffer.end());
            auto const& [value, expected] = test_data;
            message.Append(Tag::RawData, value);
            message.Finalize(1, 0);
            std::string_view message_view((char*) buffer.begin(), (char*) (buffer.begin() + message.Size()));
            EXPECT_TRUE(message_view.find(expected));
        };

        (field_test(std::get<Is>(tp)), ...);
    }
}

TEST(Test_Encode, test_append) {
    using namespace pentifica::trd::fix;

    auto test_data = std::tuple {
        std::pair {short(23),                       "96=23"},
        std::pair {(unsigned short)(35),            "96=35"},
        std::pair {int(57),                         "96=57"},
        std::pair {(unsigned int)(98),              "96=98"},
        std::pair {long(42),                        "96=42"},
        std::pair {(unsigned long)(84),             "96=84"},
        std::pair {(long long)(73),                 "96=73"},
        std::pair {(unsigned long long)(36),        "96=36"},
        std::pair {int16_t(91),                     "96=91"},
        std::pair {int32_t(-37),                    "96=-37"},
        std::pair {int64_t(-34928),                 "96=-34928"},
        std::pair {uint16_t(4),                     "96=4"},
        std::pair {uint32_t(3482),                  "96=3482"},
        std::pair {uint64_t(23873901),              "96=23873901"},
        std::pair {double(34.85),                   "96=34.85"},
        std::pair {double(-25.68),                  "96=-25.68"},
        std::pair {char('g'),                       "96=g"},
        std::pair {(char*)("this is test one"),     "96=this is test one"},
        std::pair {std::string("this is test two"), "96=this is test two"}
        };

    std::size_t const size = std::tuple_size<decltype(test_data)>::value;
    TestAppend(test_data, std::make_index_sequence<size>{});
}

namespace {
    using namespace pentifica::trd::fix;

    template<typename TupleType, std::size_t ...Is>
    void TestAppendWidth(TupleType const& tp, std::index_sequence<Is...>) {
        auto field_test = [](auto const& test_data) {
            std::array<Byte, 256> buffer;
            Encode message(MsgType::LOGON, Version::_4_2, buffer.begin(), buffer.end());
            auto& [width, value, expected] = test_data;
            message.Append(Tag::RawData, value, width);
            message.Finalize(1, 0);
            std::string_view message_view((char*) buffer.begin(), (char*) (buffer.begin() + message.Size()));
            EXPECT_TRUE(message_view.find(expected));
        };

        (field_test(std::get<Is>(tp)), ...);
    }
}

TEST(Test_Encode, test_append_width) {
    using namespace pentifica::trd::fix;

    auto test_data = std::tuple {
        std::tuple {10, short(23),                       "96=0000000023"},
        std::tuple {5,  (unsigned short)(35),            "96=00035"},
        std::tuple {8,  int(57),                         "96=00000057"},
        std::tuple {11, (unsigned int)(98),              "96=00000000098"},
        std::tuple {7,  long(42),                        "96=0000042"},
        std::tuple {12, (unsigned long)(84),             "96=000000000084"},
        std::tuple {20, (long long)(73),                 "96=00000000000000000073"},
        std::tuple {11, (unsigned long long)(36),        "96=00000000036"},
        std::tuple {7,  int16_t(91),                     "96=0000091"},
        std::tuple {23, int32_t(-37),                    "96=-0000000000000000000037"},
        std::tuple {14, int64_t(-34928),                 "96=-0000000034928"},
        std::tuple {8,  uint16_t(4),                     "96=0004"},
        std::tuple {19, uint32_t(3482),                  "96=0000000000000003482"},
        std::tuple {22, uint64_t(23873901),              "96=0000000000000023873901"}
        };

    std::size_t const size = std::tuple_size<decltype(test_data)>::value;
    TestAppendWidth(test_data, std::make_index_sequence<size>{});
}