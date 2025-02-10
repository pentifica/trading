#pragma once
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
#include    "Tags.h"

#include    <cstdint>
#include    <cstdlib>
#include    <ctime>
#include    <algorithm>
#include    <type_traits>
#include    <string>

#include    <sys/time.h>

namespace pentifica::trd::fix {
    /// @brief Encode a fix message.
    class Encode {
    public:
        Encode() = default;
        /// @brief Initialize a new fix message
        /// @param msg_type The fix message type
        /// @param version  The version of the fix message
        /// @param begin    Start of the bufer where the fix message will be encoded
        /// @param end      End+1 of the fix message buffer
        explicit Encode(MsgType msg_type, Version version, Byte* begin, Byte* end);
        Encode(Encode const&) = default;
        Encode(Encode&&) = default;
        ~Encode() = default;
        Encode& operator=(Encode const&) = default;
        Encode& operator=(Encode&&) = default;
        /// @brief Returns the current length of the encoded message
        /// @return The current length of the encoded message
        std::size_t Size() const { return next_-begin_; }
        /// @brief Returns the capacity of the message buffer
        /// @return The capacity of the message buffer
        std::size_t Capacity() const { return end_ - begin_; }
        /// @brief Append a tag=value to the end of the fix message under construction
        /// @tparam ValueType The data type of the value
        /// @param tag The tag for the fix field
        /// @param value The value for the fix field
        template<typename ValueType>
        void Append(Tag tag, ValueType&& value) {
            BeginTag(tag);
            Append(value);
            EndTag();
        }
        template<typename ValueType>
        void Append(Tag tag, ValueType&& value, uint32_t width) {
            BeginTag(tag);
            Append(value, width);
            EndTag();
        }
        /// @brief Finalize the fix message by:
        ///        -# Filling in header information
        ///        -# Appending the checksum
        /// @param seq_number The message sequence number
        /// @param last_seq_number The sequence number of the last message processed
        void Finalize(uint32_t seq_number, uint32_t last_seq_number);
    
    private:
        /// @brief Append a tag=?? to the end of the fix message under construction.
        /// @param tag The tag for the fix tag/=value entry
        /// @param width The width of the value entry
        /// @param place_holder Indicates where the value is to be placed
        void Append(Tag tag, TagWidth width, Byte*& place_holder) {
            BeginTag(tag);
            place_holder = next_;
            next_ += static_cast<std::size_t>(width);
            EndTag();
        }
        /// @brief Add a tag start to the message buffer
        /// @param tag The tag to add
        void BeginTag(Tag tag) {
            AppendUnsigned(static_cast<std::underlying_type_t<Tag>>(tag));
            Append('=');
        }
        /// @brief Add a tag terminator to the message buffer
        void EndTag() {
            *next_ = SOH;
            ++next_;
            ++checksum_;
        }
        void AppendByte(Byte byte) {
            *next_ = byte;
            ++next_;
            checksum_ += static_cast<decltype(checksum_)>(byte);
        }
        /// @brief Update the message contents
        /// @param value The update value
        void Append(MsgType value) { AppendByte(static_cast<Byte>(value)); }
        void Append(char value) { AppendByte(static_cast<Byte>(value)); }
        void Append(uint16_t value) { AppendUnsigned(value); }
        void Append(uint32_t value) { AppendUnsigned(value); }
        void Append(uint64_t value) { AppendUnsigned(value); }
        void Append(unsigned long long value) { AppendUnsigned(value); }
        void Append(int16_t value) { AppendSigned(value); }
        void Append(int32_t value) { AppendSigned(value); }
        void Append(int64_t value) { AppendSigned(value); }
        void Append(long long value) { AppendSigned(value); }
        void Append(uint16_t value, uint32_t width) { AppendUnsigned(value, width); }
        void Append(uint32_t value, uint32_t width) { AppendUnsigned(value, width); }
        void Append(uint64_t value, uint32_t width) { AppendUnsigned(value, width); }
        void Append(unsigned long long value, uint32_t width) { AppendUnsigned(value, width); }
        void Append(int16_t value, uint32_t width) { AppendSigned(value, width); }
        void Append(int32_t value, uint32_t width) { AppendSigned(value, width); }
        void Append(int64_t value, uint32_t width) { AppendSigned(value, width); }
        void Append(long long value, uint32_t width) { AppendSigned(value, width); }
        void Append(char const* value) {
            for(; *value != '\0'; value++) AppendByte(static_cast<Byte>(*value));
        }
        void Append(std::string const& value) {
            for(auto& x : value) AppendByte(static_cast<Byte>(x));
        }
        void Append(struct tm tm);
        void Append(struct timeval value);
        void Append(double value, double digits = 6.0);
        /// @brief Update the message contents with the unsigned value
        /// @tparam T The value type
        /// @param value The value to format
        template<typename T>
        void AppendUnsigned(T value) {
            static_assert(std::is_same_v<decltype(value), uint16_t>
                || std::is_same_v<decltype(value), uint32_t>
                || std::is_same_v<decltype(value), uint64_t>
                || std::is_same_v<decltype(value), unsigned long long>, "Must be an unsigned value");
            constexpr T ten{10};
            auto begin = next_;
            do {
                AppendByte(static_cast<Byte>(value % ten) + '0');
                value /= ten;
            }   while(value != 0);
    
            for(auto end = next_ - 1; begin < end; ++begin, --end) {
                std::swap(*begin, *end);
            }
        }
        /// @brief Update the message contents with the unsigned value front-padding with '0'
        /// @tparam T The unsigned value type
        /// @param value The value to format
        /// @param width The width to format
        template<typename T>
        void AppendUnsigned(T value, uint32_t width) {
            static_assert(std::is_same_v<decltype(value), uint16_t>
                || std::is_same_v<decltype(value), uint32_t>
                || std::is_same_v<decltype(value), uint64_t>
                || std::is_same_v<decltype(value), unsigned long long>, "Must be an unsigned value");
            constexpr T ten{10};
    
            auto begin = next_;
            next_ += width;
            auto end = next_;
            std::fill(begin, end, '0');
    
            while(--end > begin) {
                *end += static_cast<Byte>(value % ten);
                checksum_ += static_cast<uint32_t>(*end);
                value /= ten;
            }
        }
        /// @brief Update the message contents with the signed value
        /// @tparam T The signed value type
        /// @param value The value to format
        template<typename T>
        void AppendSigned(T value) {
            static_assert(std::is_same_v<decltype(value), int16_t>
                || std::is_same_v<decltype(value), int32_t>
                || std::is_same_v<decltype(value), int64_t>
                || std::is_same_v<decltype(value), long long>, "Must be a signed value");
            if(value < 0) {
                Append('-');
                value = std::abs(value);
            }
            if constexpr(std::is_same_v<decltype(value), int32_t>)
                AppendUnsigned(static_cast<uint32_t>(value));
            else
                AppendUnsigned(static_cast<uint64_t>(value));
        }
        /// @brief Update the message contents with the signed value front-padding with '0'
        /// @tparam T The signed value type
        /// @param value The value to format
        /// @param width The width to format
        template<typename T>
        void AppendSigned(T value, uint32_t width) {
            static_assert(std::is_same_v<decltype(value), int16_t>
                || std::is_same_v<decltype(value), int32_t>
                || std::is_same_v<decltype(value), int64_t>
                || std::is_same_v<decltype(value), long long>, "Must be a signed value");
            if(value < 0) {
                Append('-');
                value = std::abs(value);
            }
            if constexpr(std::is_same_v<decltype(value), int32_t>)
                AppendUnsigned(static_cast<uint32_t>(value, width));
            else
                AppendUnsigned(static_cast<uint64_t>(value, width));
        }
        template<typename T>
        void InsertUnsigned(Byte* begin, TagWidth width, T value) {
            auto save_next = next_;
            next_ = begin;
            AppendUnsigned(value, static_cast<std::underlying_type_t<TagWidth>>(width));
            next_ = save_next;
        }
        template<typename T>
        void InsertSigned(Byte* begin, TagWidth width, T value) {
            auto save_next = next_;
            next_ = begin;
            AppendSigned(value, width);
            next_ = save_next;
        }
    private:
        Byte* begin_{};
        Byte* end_{};
        Byte* next_{};
        Byte* msg_type_{};
        Byte* body_length_field_{};
        Byte* body_length_field_end_{};
        Byte* msg_seq_num_field_{};
        Byte* sending_time_field_{};
        Byte* last_msg_seq_num_processed_field_{};
        std::uint32_t checksum_{};
    };  
}