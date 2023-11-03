#include    <parser/fix/Parser.h>
#include    <parser/fix/Tags.h>

#include    <gtest/gtest.h>

#include    <string>
#include    <string_view>
#include    <algorithm>
#include    <numeric>  
#include    <iostream> 

namespace test_message {
    char const* const logon = "8=FIX.4.4|9=88|35=A|49=fix_client|56=BTNL_PF|34=1|52=20061124-15:47:02.951|98=0|108=30|553=10|554=AUTHTOKEN|141=Y|10=XXX|";
    char const* const heartbeat = "8=FIX.4.4|9=57|35=0|34=2|49=BTNL_DC|56=BTNL_DC|52=20210525-16:59:02.564|10=XXX|";
    char const* const test_request = "8=FIX.4.4|9=75|35=1|49=BTNL_PF|56=fix_client|34=2|52=20061124-15:50:32.215|112=PostLogon_00001|10=XXX|";
    char const* const logout = "8=FIX.4.4|9=95|35=5|49=BTNL_PF|56=fix_client|34=25|52=20061124-15:59:50.524|58=NormalLogoutInitiatedByCounterparty|10=XXX|";
    char const* const incomplete = "8=FIX.4.2|9=60";
    char const* const no_message_type = "8=FIX.4.2|9=88|39=abcd|";
    char const* const empty = "";
    char const* const basic = "8=FIX.4.4|9=0|10=XXX|";
    char const* const no_begin_string = "7=anc|";
    char const* const no_body_length = "8=FIX.4.4|35=A|";
    char const* const invalid_checksum = "8=FIX.4.2|9=0|10=111|";
}

namespace {
    std::string PrepMessage(char const* message, bool calculate_checksum = true) {
        using namespace pentifica::trd::fix;
        std::string_view view(message, message + strlen(message));
        std::string fix;
        fix.reserve(view.size());
        for(auto c : view) {
            fix += (c == '|') ? '\1' : c;
        }

        if(!calculate_checksum) return fix;

        auto body_length = fix.find("9=");
        if(body_length == std::string::npos) return fix;

        auto message_start =fix.find('\1', body_length + 2) + 1;

        auto checksum_start = fix.find("10=");
        if(checksum_start == std::string::npos) return fix;

        auto checksum = std::accumulate(fix.begin() + message_start, fix.begin() + checksum_start,
            uint32_t(0)) % 256;

        checksum_start += 3;
        auto checksum_end = checksum_start + static_cast<decltype(checksum_start)>(TagWidth::CheckSum);

        std::fill(fix.begin() + checksum_start, fix.begin() + checksum_end, '0');
        while(checksum_end-- != checksum_start) {
            *(fix.begin() + checksum_end) += checksum % 10;
            checksum /= 10;
        }

        return fix;
    }
}

TEST(Test_Parser, basic) {
    using namespace pentifica::trd::fix;

    auto const& message = PrepMessage(test_message::basic);

    auto begin = reinterpret_cast<const Byte*>(message.c_str());
    auto end = begin + message.size();

    Parser parser(begin, end);

    EXPECT_EQ(Version::_4_4, parser.GetVersion());
    EXPECT_EQ(0, parser.GetBodyLength());
    EXPECT_EQ(0, parser.GetChecksum());
}
//  ----------------------------------------------------------------------------
//
void
TestExceptionHelper(const char* raw_data, const char* exception_message, bool checksum_calc = true) {
    using namespace pentifica::trd::fix;

    auto const& message = PrepMessage(raw_data, checksum_calc);
    auto begin = reinterpret_cast<Byte const*>(message.c_str());
    auto end = begin + message.size();

    try {
        Parser parser(begin, end);
        FAIL();
    }
    catch(ParseSyntax const& e) {
        EXPECT_STREQ(e.what(), exception_message);
    }
    catch(...) {
        FAIL();
    }
}
//  ----------------------------------------------------------------------------
//
TEST(Test_Parser, empty) {
    TestExceptionHelper(test_message::empty, "Empty FIX message", false);
}
//  ----------------------------------------------------------------------------
//
TEST(Test_Parser, no_begin_string) {
    TestExceptionHelper(test_message::no_begin_string, "Tag not BeginString");
}
//  ----------------------------------------------------------------------------
//
TEST(Test_Parser, no_body_length) {
    TestExceptionHelper(test_message::no_body_length, "Expected BodyLength");
}
//  ----------------------------------------------------------------------------
//
TEST(Test_Parser, invalid_checksum) {
    TestExceptionHelper(test_message::invalid_checksum, "Invalid checksum", false);
}