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
#include    <string>
#include    <string_view>
#include    <numeric>
#include    <algorithm>
#include    <iterator>
#include    <cmath>
#include    <type_traits>
#include    <cstdint>

namespace pentifica::trd {
    template<typename T>
    T numeric_converter(std::string_view const& view) {
        constexpr T mult{10};
        T value{};
        for(auto digit : view) {
            value = (value * mult) + (digit - '0');
        }
        return value;
    }
    template<typename T>
    T real_converter(std::string_view const& view) {
        auto begin = view.begin();

        T sign{1.0};
        if(*begin == '-') {
            sign = -sign;
            ++begin;
        }

        auto decimal = std::find(begin, view.end(), '.');
        T result = numeric_converter<T>(std::string_view(begin, decimal));

        auto digits = std::distance(decimal, view.end()) - 1;

        if(digits > 0) {
            result += numeric_converter<T>(std::string_view(decimal+1, view.end()))
                / pow(10, digits);
        }

        return sign * result;
    }
    /// @brief Workaround to allow static_assert to fail
    /// @tparam T   Placeholder
    template<typename T> struct Unsupported : std::false_type {};
    template<typename T>
    T translate(std::string_view const& view) {

        //  uin8_t, uint16_t, uin3_t, uint64_t
        //  unsignd, unsigned char, unsigned long, unsigned long long
        if constexpr(
               std::is_same_v<T, uint8_t>
            || std::is_same_v<T, uint16_t>
            || std::is_same_v<T, uint32_t>
            || std::is_same_v<T, uint64_t>
            || std::is_same_v<T, unsigned>
            || std::is_same_v<T, unsigned char>
            || std::is_same_v<T, unsigned long>
            || std::is_same_v<T, unsigned long long>
        )
            return numeric_converter<T>(view);

        //  int8_t, int16_t, int32_t, it64_t
        //  int, char,long, long long
        else if constexpr(
               std::is_same_v<T, int8_t>
            || std::is_same_v<T, int16_t>
            || std::is_same_v<T, int32_t>
            || std::is_same_v<T, int64_t>
            || std::is_same_v<T, int>
            || std::is_same_v<T, char>
            || std::is_same_v<T, long>
            || std::is_same_v<T, long long>
        ) {
            if(view[0] == '-') {
                return -numeric_converter<T>(std::string_view(view.begin()+1, view.end()));
            }
            return numeric_converter<T>(view);
        }

        //  float, double, long double
        else if constexpr(
               std::is_same_v<T, float>
            || std::is_same_v<T, double>
            || std::is_same_v<T, long double>
        ) {
            return real_converter<T>(view);
        }

        //  unhandled
        else {
            static_assert(Unsupported<T>::value, "Unsupported type");
        }

        return {};
    }
    template<>
    std::string translate<std::string>(std::string_view const& view) {
        return std::string(view.begin(), view.end());
    }
}