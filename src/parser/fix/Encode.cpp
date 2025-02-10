/// @copyright {2023, Russell J. Fleming. All rights reserved.}
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
#include    "Encode.h"
#include    "Utility.h"

#include    <stdexcept>
#include    <cmath>
#include    <ctime>
#include    <string>

namespace pentifica::trd::fix {
    Encode::Encode(MsgType msg_type, Version version, Byte* begin, Byte* end) :
        begin_{begin},
        end_{end},
        next_{begin}
    {
        Append(Tag::BeginString, VersionMapping::Map(version));
        Append(Tag::BodyLength, TagWidth::BodyLength, body_length_field_);
        body_length_field_end_ = next_;
    
        Append(Tag::MsgType, msg_type);
        Append(Tag::MsgSeqNum, TagWidth::MsgSeqNum, msg_seq_num_field_);
        Append(Tag::SendingTime, TagWidth::SendingTime, sending_time_field_);
        Append(Tag::LastMsgSeqNumProcessed, TagWidth::LastMsgSeqNumProcessed, last_msg_seq_num_processed_field_);
    }
    void
    Encode::Finalize(uint32_t seq_number, uint32_t last_seq_number) {
        std::size_t length = next_ - body_length_field_end_;
        InsertUnsigned(body_length_field_, TagWidth::BodyLength, length);
    
        InsertUnsigned(msg_seq_num_field_, TagWidth::MsgSeqNum, seq_number);
        InsertUnsigned(last_msg_seq_num_processed_field_, TagWidth::LastMsgSeqNumProcessed, last_seq_number);
    
        //  timestamp the message completion
        auto save_next = next_;
        struct timeval tv;
        gettimeofday(&tv, 0);
        next_ = sending_time_field_;
        Append(tv);
        next_ = save_next;
    
        //  append checksum
        *(next_ + 0) = '1';
        *(next_ + 1) = '0';
        *(next_ + 2) = '=';
    
        auto const checksum = checksum_ % 256;
    
        *(next_ + 3) = '0' +  (checksum / 100);
        *(next_ + 4) = '0' + ((checksum % 100) / 10);
        *(next_ + 5) = '0' + ((checksum % 100) % 10);
    
        *(next_ + 6) = SOH;
    
        next_ += 6;
    }
    void
    Encode::Append(double value, double digits) {
        //  handle < 0
        //
        bool const negative{value < 0.0};
        if(negative) {
            Append('-');
            value = -value;
        }
    
        //  split into whole and fraction
        //
        double whole;
        double fraction = (modf(value, &whole) * pow(10, digits)) + 0.9;
    
        //  append
        //
        AppendUnsigned(static_cast<uint64_t>(whole));
        Append('.');
        AppendUnsigned(static_cast<uint64_t>(fraction));
    }
    void
    Encode::Append(struct tm value) {
        value.tm_year -= 100;
        Append('2');
        Append('0');
        Append(static_cast<char>((value.tm_year / 10) + '0'));
        Append(static_cast<char>((value.tm_year % 10) + '0'));
    
        ++value.tm_mon;
        Append(static_cast<char>((value.tm_mon / 10) + '0'));
        Append(static_cast<char>((value.tm_mon % 10) + '0'));
    
        Append(static_cast<char>((value.tm_mday / 10) + '0'));
        Append(static_cast<char>((value.tm_mday % 10) + '0'));
    
        Append('-');
    
        Append(static_cast<char>((value.tm_hour / 10) + '0'));
        Append(static_cast<char>((value.tm_hour % 10) + '0'));
    
        Append(':');
    
        Append(static_cast<char>((value.tm_min / 10) + '0'));
        Append(static_cast<char>((value.tm_min % 10) + '0'));
    
        Append(':');
    
        Append(static_cast<char>((value.tm_sec / 10) + '0'));
        Append(static_cast<char>((value.tm_sec % 10) + '0'));
    }
    void
    Encode::Append(struct timeval value) {
        struct tm tm;
        gmtime_r(&value.tv_sec, &tm);
    
        Append(tm);
        Append('.');
        Append(value.tv_usec / 1000);
    }
}