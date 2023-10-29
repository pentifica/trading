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

#include    <unordered_map>
#include    <type_traits>
#include    <tuple>
#include    <optional>
#include    <stdexcept>
#include    <string_view>

namespace pentifica::trd::fix {
    struct TagHash {
        using HashType = std::underlying_type_t<Tag>;
        HashType operator()(Tag tag) { return static_cast<HashType>(tag); }
    };
    using TagFinder = std::unordered_map<Tag, Byte*, TagHash>;

    struct ParseIncomplete : public std::length_error {
        using std::length_error::length_error;
    };

    struct ParseSyntax : public std::logic_error {
        using std::logic_error::logic_error;
    };

    class Parser {
    public:
        /// @brief Initialize the parser with the message to parse
        /// @param begin    Start of message
        /// @param end      End of message + 1
        explicit Parser(Byte const* begin, Byte const* end);
        using TagInfo = std::tuple<Tag, std::string_view>;
        using ParsedTag = std::optional<TagInfo>;
        /// @brief Returns the next tag in the message
        /// @return     The next tage in the message
        ParsedTag NextTag();
        auto GetVersion() const {return version_; }
        auto GetBodyLength() const { return body_length_; }
        auto GetChecksum() const { return checksum_; }

    private:
        /// @brief  Parse and validate the BEGIN field
        void BeginString();
        /// @brief  Parse and validate the BODY LENGTH field
        void BodyLength();
        /// @brief  Parse and validate the CHECKSUM field
        void Checksum();

    private:
        Byte const* const begin_{};
        Byte const* const end_{};
        Byte const* next_{};
        std::uint32_t checksum_{};
        Version version_{Version::Unknown};
        std::uint32_t body_length_{};
    };
}