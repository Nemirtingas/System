/*
 * Copyright (C) Nemirtingas
 * This file is part of System.
 *
 * System is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * System is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with System; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "System_internals.h"

#include <System/Encoding.hpp>
#include <utfcpp/utf8.h>

#include <string.h>
#include <algorithm>

namespace System {
namespace Encoding {

namespace details {
    template<typename T, size_t s = sizeof(typename T::value_type)>
    struct string_deducer
    {
        static std::string  convert_string(T const& str) = delete;
        static std::wstring convert_wstring(T const& str) = delete;
    };

    template<typename T>
    struct string_deducer<T, 1>
    {
        static std::string convert_string(T const& str)
        {
            return std::string(std::begin(str), std::end(str));
        }

        static std::wstring convert_wstring(std::string const& str)
        {
            std::wstring r(std::begin(str), std::end(str));
            return r;
        }

        static std::wstring convert_wstring(std::string_view str)
        {
            std::wstring r(std::begin(str), std::end(str));
            return r;
        }
    };

    template<typename T>
    struct string_deducer<T, 2>
    {
        static std::string convert_string(T const& str)
        {
            std::string r;
            utf8::utf16to8(std::begin(str), std::end(str), std::back_inserter(r));
            return r;
        }

        static std::wstring convert_wstring(std::string const& str)
        {
            std::wstring r;
            utf8::utf8to16(std::begin(str), std::end(str), std::back_inserter(r));
            return r;
        }

        static std::wstring convert_wstring(std::string_view str)
        {
            std::wstring r;
            utf8::utf8to16(std::begin(str), std::end(str), std::back_inserter(r));
            return r;
        }
    };

    template<typename T>
    struct string_deducer<T, 4>
    {
        static std::string convert_string(T const& str)
        {
            std::string r;
            utf8::utf32to8(std::begin(str), std::end(str), std::back_inserter(r));
            return r;
        }

        static std::wstring convert_wstring(std::string const& str)
        {
            std::wstring r;
            utf8::utf8to32(std::begin(str), std::end(str), std::back_inserter(r));
            return r;
        }

        static std::wstring convert_wstring(std::string_view str)
        {
            std::wstring r;
            utf8::utf8to32(std::begin(str), std::end(str), std::back_inserter(r));
            return r;
        }
    };
}

std::wstring Utf8ToWChar(std::string const& str)
{
    return details::string_deducer<std::wstring>::convert_wstring(str);
}

std::wstring Utf8ToWChar(std::string_view str)
{
    return details::string_deducer<std::wstring>::convert_wstring(str);
}

std::u16string Utf8ToUtf16(std::string const& str)
{
    return utf8::utf8to16(str);
}

std::u32string Utf8ToUtf32(std::string const& str)
{
    return utf8::utf8to32(str);
}

std::u16string Utf8ToUtf16(std::string_view str)
{
    return utf8::utf8to16(str);
}

std::u32string Utf8ToUtf32(std::string_view str)
{
    return utf8::utf8to32(str);
}

std::string WCharToUtf8(std::wstring const& str)
{
    return details::string_deducer<std::wstring>::convert_string(str);
}

std::string WCharToUtf8(std::wstring_view str)
{
    return details::string_deducer<std::wstring_view>::convert_string(str);
}

std::string Utf16ToUtf8(std::u16string const& str)
{
    return utf8::utf16to8(str);
}

std::string Utf32ToUtf8(std::u32string const& str)
{
    return utf8::utf32to8(str);
}

std::string Utf16ToUtf8(std::u16string_view str)
{
    return utf8::utf16to8(str);
}

std::string Utf32ToUtf8(std::u32string_view str)
{
    return utf8::utf32to8(str);
}

size_t EncodedLength(std::string const& str)
{
    return utf8::distance(str.begin(), str.end());
}

size_t EncodedLength(std::string_view str)
{
    return utf8::distance(str.begin(), str.end());
}

namespace Base64
{

static char constexpr Base64Alphabet[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" };
static signed char constexpr Base64Inverse[] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //   0-15
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //  16-31
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, //  32-47
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, //  48-63
        -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, //  64-79
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, //  80-95
        -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, //  96-111
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, // 112-127
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 128-143
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 144-159
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 160-175
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 176-191
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 192-207
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 208-223
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 224-239
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  // 240-255
};

static char constexpr Base64UrlAlphabet[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_" };
static signed char constexpr Base64UrlInverse[] = {
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //   0-15
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //  16-31
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, //  32-47
         52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, //  48-63
         -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, //  64-79
         15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63, //  80-95
         -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, //  96-111
         41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, // 112-127
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 128-143
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 144-159
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 160-175
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 176-191
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 192-207
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 208-223
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 224-239
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  // 240-255
};


std::size_t EncodeWithAlphabet(void* dest, void const* src, std::size_t len, bool padding, const char* alphabet)
{
    char* out = static_cast<char*>(dest);
    char const* in = static_cast<char const*>(src);

    for (auto n = len / 3; n--;)
    {
        *out++ = alphabet[(in[0] & 0xfc) >> 2];
        *out++ = alphabet[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
        *out++ = alphabet[((in[2] & 0xc0) >> 6) + ((in[1] & 0x0f) << 2)];
        *out++ = alphabet[in[2] & 0x3f];
        in += 3;
    }

    switch (len % 3)
    {
    case 2:
        *out++ = alphabet[(in[0] & 0xfc) >> 2];
        *out++ = alphabet[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
        *out++ = alphabet[(in[1] & 0x0f) << 2];
        if (padding)
            *out++ = '=';
        break;

    case 1:
        *out++ = alphabet[(in[0] & 0xfc) >> 2];
        *out++ = alphabet[((in[0] & 0x03) << 4)];
        if (padding)
        {
            *out++ = '=';
            *out++ = '=';
        }
        break;

    case 0:
        break;
    }

    return out - static_cast<char*>(dest);
}

std::pair<std::size_t, std::size_t> DecodeWithAlphabet(void* dest, char const* src, std::size_t len, const signed char* alphabet)
{
    char* out = static_cast<char*>(dest);
    auto in = reinterpret_cast<unsigned char const*>(src);
    unsigned char c3[3], c4[4];
    int i = 0;
    int j = 0;

    while (len-- && *in != '=')
    {
        auto const v = alphabet[*in];
        if (v == -1)
            break;
        ++in;
        c4[i] = v;
        if (++i == 4)
        {
            c3[0] = (c4[0] << 2) + ((c4[1] & 0x30) >> 4);
            c3[1] = ((c4[1] & 0xf) << 4) + ((c4[2] & 0x3c) >> 2);
            c3[2] = ((c4[2] & 0x3) << 6) + c4[3];

            for (i = 0; i < 3; i++)
                *out++ = c3[i];
            i = 0;
        }
    }

    if (i)
    {
        c3[0] = (c4[0] << 2) + ((c4[1] & 0x30) >> 4);
        c3[1] = ((c4[1] & 0xf) << 4) + ((c4[2] & 0x3c) >> 2);
        c3[2] = ((c4[2] & 0x3) << 6) + c4[3];

        for (j = 0; j < i - 1; j++)
            *out++ = c3[j];
    }

    return { out - static_cast<char*>(dest), in - reinterpret_cast<unsigned char const*>(src) };
}

std::size_t Encode(void* dest, void const* src, std::size_t len, bool padding)
{
    return EncodeWithAlphabet(dest, src, len, padding, Base64Alphabet);
}

std::pair<std::size_t, std::size_t> Decode(void* dest, char const* src, std::size_t len)
{
    return DecodeWithAlphabet(dest, src, len, Base64Inverse);
}

std::size_t UrlEncode(void* dest, void const* src, std::size_t len, bool padding)
{
    return EncodeWithAlphabet(dest, src, len, padding, Base64UrlAlphabet);
}

std::pair<std::size_t, std::size_t> UrlDecode(void* dest, char const* src, std::size_t len)
{
    return DecodeWithAlphabet(dest, src, len, Base64UrlInverse);
}

}// namespace Base64

}// namespace Encoding
}// namespace System
