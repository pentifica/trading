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
#include    <parser/Converter.h>

#include    <cctype>

namespace {
    template<typename T, typename L>
    auto make_sv(T const* start, L len) {
        return std::string_view(reinterpret_cast<char const*>(start),
            static_cast<std::string_view::size_type>(len));
    }
}

namespace pentifica::trd::fix {
    //  ================================================================
    //
    Parser::Parser(Byte const* begin, Byte const* end) :
        begin_{begin},
        end_{end},
        next_(begin)
    {
        if(end <= begin) throw ParseSyntax("Empty FIX message");
        BeginString();
        BodyLength();
        Checksum();
    }
    //  ================================================================
    //
    void
    Parser::BeginString() {
        auto const& next_tag{NextTag()};
        if(!next_tag) throw ParseSyntax("Expected BeginString");

        auto const& [tag, view] = next_tag.value();
        if(tag != Tag::BeginString) throw ParseSyntax("Tag not BeginString");

        version_ = VersionMapping::Map(view);
    }
    //  ================================================================
    //
    void
    Parser::BodyLength() {
        auto const& next_tag{NextTag()};
        if(!next_tag) throw ParseSyntax("Expected BodyLength");

        auto const& [tag, view] = next_tag.value();
        if(tag != Tag::BodyLength) throw ParseSyntax("Expected BodyLength");

        //body_length_ = translate<decltype(body_length_)>(view);

        auto computed_end = next_ + body_length_ + 4 + static_cast<decltype(body_length_)>(TagWidth::CheckSum);
        if(computed_end != end_) throw ParseSyntax("Incorrect BodyLength");
    }
    //  ================================================================
    //
    void
    Parser::Checksum() {
        auto checksum_start = next_ + body_length_;
        checksum_ = translate<decltype(checksum_)>(make_sv(checksum_start + 3, TagWidth::CheckSum));

        auto computed_checksum = std::accumulate(next_, checksum_start,
            static_cast<decltype(checksum_)>(0)) % 256;
        if(computed_checksum != checksum_) throw ParseSyntax("Invalid checksum");
    }
    //  ================================================================
    //
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

        auto const begin = ++next_;

        for(; next_ != end_ && *next_ != SOH; ++next_)
            checksum_ += static_cast<decltype(checksum_)>(*next_);

        if(next_ == end_) throw ParseIncomplete("Missing SOH");

        if constexpr(SOH != Byte{1})
            checksum_ += static_cast<uint32_t>(SOH);
        else
            checksum_++;

        auto count = (next_ - begin);
        ++next_;

        return std::make_tuple(static_cast<Tag>(tag), make_sv(begin, count));
    }
}