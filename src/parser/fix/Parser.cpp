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
#include    "Parser.h"
#include    "Utility.h"

#include    <cctype>

namespace pentifica::trd::fix {
    Parser::Parser(Byte const* begin, Byte const* end, TagFinder& tags) :
        begin_{begin},
        end_{end},
        tags_{tags}
    {
        if(end <= begin) throw ParseIncomplete("Empty FIX message");
        BeginString();
        BodyLength();
        Checksum();
    }

    void
    Parser::BeginString() {
        auto const& next_tag{NextTag()};

        auto const [tag, begin, end] = next_tag.value();

        if(tag != Tag::BeginString) throw ParseSyntax("Expected BeginString");

        version_ = VersionMapping::Map(begin, end);
    }

    Parser::ParsedTag
    Parser::NextTag() {
        if(next_ == end_) return ParsedTag();

        constexpr Byte offset{static_cast<Byte>('0')};
        constexpr Byte term{static_cast<Byte>('=')};

        using ValueType = std::underlying_type_t<Tag>;

        ValueType tag{};
        for(; next_ != end_ && *next_ != term; next_++) {
            if(!isdigit(*next_))
                throw ParseSyntax("Non-digit in tag specification");
            checksum_ += static_cast<uint32_t>(*next_);
            tag *= 10;
            tag += static_cast<ValueType>(*next_ - offset);
        }

        if(next_ == end_)
            throw ParseIncomplete("Missing '='");
        if(*next_ != term)
            throw ParseSyntax("Missing '=' tag terminator");

        checksum_ += term;

        auto begin = ++next_;

        for(; next_ != end_ && *next_ != SOH; ++next_)
            checksum_ += static_cast<uint32_t>(*next_);

        if(next_ == end_) throw ParseIncomplete("Missing SOH");

        if constexpr(SOH != Byte{1})
            checksum_ += static_cast<uint32_t>(SOH);
        else
            checksum_++;

        auto end = next_;
        ++next_;

        return {{static_cast<Tag>(tag), begin, end}};
    }
}