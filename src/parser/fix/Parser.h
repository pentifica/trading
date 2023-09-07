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
        explicit Parser(Byte const* begin, Byte const* end, TagFinder& tags);
        using TagInfo = std::tuple<Tag, Byte const*, Byte const*>;
        using ParsedTag = std::optional<TagInfo>;
        ParsedTag NextTag();

    private:
        void BeginString();
        void BodyLength();
        void Checksum();

    private:
        Byte const* begin_{};
        Byte const* const end_;
        TagFinder& tags_;
        Byte const* next_{};
        std::uint32_t checksum_{};
        Version version_{Version::Unknown};
        std::uint32_t body_length_{};
    };
}