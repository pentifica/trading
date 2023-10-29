#include    <parser/fix/Parser.h>
#include    <parser/fix/Tags.h>

#include    <gtest/gtest.h>

#include    <string>
#include    <string_view>
#include    <algorithm>
#include    <numeric>  
#include    <iostream> 

namespace test_message {
    char const* logon = "8=FIX.4.4|9=88|35=A|49=fix_client|56=BTNL_PF|34=1|52=20061124-15:47:02.951|98=0|108=30|553=10|554=AUTHTOKEN|141=Y|10=XXX|";
    char const* heartbeat = "8=FIX.4.4|9=57|35=0|34=2|49=BTNL_DC|56=BTNL_DC|52=20210525-16:59:02.564|10=XXX|";
    char const* test_request = "8=FIX.4.4|9=75|35=1|49=BTNL_PF|56=fix_client|34=2|52=20061124-15:50:32.215|112=PostLogon_00001|10=XXX|";
    char const* logout = "8=FIX.4.4|9=95|35=5|49=BTNL_PF|56=fix_client|34=25|52=20061124-15:59:50.524|58=NormalLogoutInitiatedByCounterparty|10=XXX|";
    char const* incomplete = "8=FIX.4.2|9=60";
    char const* empty = "";
    char const* no_body_length = "8=FIX.4.4|35=A";
    char const* no_message_type = "8=FIX.4.2|9=88|39=abcd|";
    char const* no_begin_string = "7=anc|";
    char const* basic = "8=FIX.4.4|9=0|10=XXX|";
}

namespace {
    std::string PrepMessage(char const* message) {
        using namespace pentifica::trd::fix;
        std::string_view view(message, message + strlen(message));
        std::string fix;
        fix.reserve(view.size());
        for(auto c : view) {
            fix += (c == '|') ? '\1' : c;
        }

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

    ASSERT_EQ(Version::_4_4, parser.GetVersion());
    ASSERT_EQ(0, parser.GetBodyLength());
    ASSERT_EQ(0, parser.GetChecksum());
}
