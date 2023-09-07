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

#include    <string>
#include    <string_view>
#include    <unordered_map>
#include    <type_traits>

namespace pentifica::trd::fix {
    struct VersionMapping {
        static std::string const& Map(Version version) {
            auto const entry = version_to_name.find(version);
            return (entry != version_to_name.end()) ? entry->second : unknown;
        }
        static Version Map(Byte const* begin, Byte const* end) {
            std::string const view(reinterpret_cast<char const*>(begin), end - begin);
            auto const entry = name_to_version.find(view);
            return (entry != name_to_version.end()) ? entry->second : Version::Unknown;
        }
    private:
        static std::string const FIX_4_2;
        static std::string const unknown;
        static std::unordered_map<std::string, Version> const name_to_version;
        static std::unordered_map<Version, std::string> const version_to_name;
    };
}