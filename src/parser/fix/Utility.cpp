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
#include    "Utility.h"

namespace pentifica::trd::fix {
    std::string const VersionMapping::FIX_4_2{"FIX.4.2"};
    std::string const VersionMapping::FIX_4_4{"FIX.4.4"};
    std::string const VersionMapping::unknown{"UNKNOWN"};
    std::unordered_map<std::string, Version> const VersionMapping::name_to_version = {
        {FIX_4_2, Version::_4_2},
        {FIX_4_4, Version::_4_4},
        {unknown, Version::Unknown},
    };
    std::unordered_map<Version, std::string> const VersionMapping::version_to_name = {
        {Version::_4_2, FIX_4_2},
        {Version::_4_4, FIX_4_4},
        {Version::Unknown, unknown},
    };
}