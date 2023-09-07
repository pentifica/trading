#include    <parser/Converter.h>

#include    <gtest/gtest.h>

#include    <string>
#include    <string_view>
#include    <vector>

namespace {
    template<typename T>
    struct TestData {
        std::string text_;
        T expected_;
    };
}

TEST(Test_Converter, test_uint8_t) {
    using namespace pentifica::trd;

    std::vector<TestData<uint8_t>> tests = {
        {"0", 0},
        {"8", 8},
        {"200", 200},
        {"255", 255},
        {"256", 0},
        {"257", 1}
    };

    for(auto const& [text, expected] : tests) {
        auto actual = translate<uint8_t>(std::string_view(text.begin(), text.end()));
        EXPECT_EQ(expected, actual);
    }
}

TEST(Test_Converter, test_uint16_t) {
    using namespace pentifica::trd;

    std::vector<TestData<uint16_t>> tests = {
        {"0", 0},
        {"8", 8},
        {"200", 200},
        {"65535", 65535},
        {"65536", 0},
        {"65537", 1}
    };

    for(auto const& [text, expected] : tests) {
        auto actual = translate<uint16_t>(std::string_view(text.begin(), text.end()));
        EXPECT_EQ(expected, actual);
    }
}

TEST(Test_Converter, test_uint32_t) {
    using namespace pentifica::trd;

    std::vector<TestData<uint32_t>> tests = {
        {"0", 0},
        {"8", 8},
        {"200", 200},
        {"4294967295", 4294967295},
        {"4294967296", 0},
        {"4294967297", 1}
    };

    for(auto const& [text, expected] : tests) {
        auto actual = translate<uint32_t>(std::string_view(text.begin(), text.end()));
        EXPECT_EQ(expected, actual);
    }
}

TEST(Test_Converter, test_uint64_t) {
    using namespace pentifica::trd;

    std::vector<TestData<uint64_t>> tests = {
        {"0", 0},
        {"8", 8},
        {"200", 200},
        {"18446744073709551615", 18446744073709551615ull},
        {"18446744073709551616", 0},
        {"18446744073709551617", 1}
    };

    for(auto const& [text, expected] : tests) {
        auto actual = translate<uint64_t>(std::string_view(text.begin(), text.end()));
        EXPECT_EQ(expected, actual);
    }
}

TEST(Test_Converter, test_int8_t) {
    using namespace pentifica::trd;

    std::vector<TestData<int8_t>> tests = {
        {"0", 0},
        {"8", 8},
        {"-8", -8},
        {"120", 120},
        {"-120", -120},
        {"127", 127},
        {"128", -128},
        {"-128", -128},
        {"-129", 127}
    };

    for(auto const& [text, expected] : tests) {
        auto actual = translate<int8_t>(std::string_view(text.begin(), text.end()));
        EXPECT_EQ(expected, actual);
    }
}

TEST(Test_Converter, test_int16_t) {
    using namespace pentifica::trd;

    std::vector<TestData<int16_t>> tests = {
        {"0", 0},
        {"8", 8},
        {"-8", -8},
        {"200", 200},
        {"-200", -200},
        {"32767", 32767},
        {"32768", -32768},
        {"-32768", -32768},
        {"-32769", 32767},
    };

    for(auto const& [text, expected] : tests) {
        auto actual = translate<int16_t>(std::string_view(text.begin(), text.end()));
        EXPECT_EQ(expected, actual);
    }
}

TEST(Test_Converter, test_int32_t) {
    using namespace pentifica::trd;

    std::vector<TestData<int32_t>> tests = {
        {"0", 0},
        {"8", 8},
        {"-8", -8},
        {"200", 200},
        {"-200", -200},
        {"2147483647", 2147483647},
        {"-2147483648", -2147483648},
        {"2147483648", -2147483648},
        {"-2147483649", 2147483647},
    };

    for(auto const& [text, expected] : tests) {
        auto actual = translate<int32_t>(std::string_view(text.begin(), text.end()));
        EXPECT_EQ(expected, actual);
    }
}

TEST(Test_Converter, test_float) {
    using namespace pentifica::trd;

    std::vector<TestData<float>> tests = {
        {"0", 0},
        {"8.500", 8.5},
        {"-8.439", -8.439},
        {"120.78030", 120.78030},
        {"-120.831", -120.831},
    };

    for(auto const& [text, expected] : tests) {
        auto actual = translate<float>(std::string_view(text.begin(), text.end()));
        EXPECT_FLOAT_EQ(expected, actual);
    }
}

TEST(Test_Converter, test_double) {
    using namespace pentifica::trd;

    std::vector<TestData<double>> tests = {
        {"0", 0},
        {"8.500", 8.5},
        {"-8.439", -8.439},
        {"120.78030", 120.78030},
        {"-120.831", -120.831},
    };

    for(auto const& [text, expected] : tests) {
        auto actual = translate<double>(std::string_view(text.begin(), text.end()));
        EXPECT_DOUBLE_EQ(expected, actual);
    }
}

TEST(Test_Converter, test_string) {
    using namespace pentifica::trd;

    std::vector<TestData<std::string>> tests = {
        {"0", "0"},
        {"8.500", "8.500"},
        {"-8.439", "-8.439"},
        {"120.78030", "120.78030"},
        {"-120.831", "-120.831"},
    };

    for(auto const& [text, expected] : tests) {
        auto actual = translate<std::string>(std::string_view(text.begin(), text.end()));
        EXPECT_EQ(expected, actual);
    }
}