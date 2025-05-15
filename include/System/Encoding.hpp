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
 * License along with the System; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <string>
#include <string_view>
#include <cstdint>

namespace System {
namespace Encoding {

std::wstring Utf8ToWChar(std::string const& str);

std::wstring Utf8ToWChar(std::string_view str);

std::u16string Utf8ToUtf16(std::string const& str);

std::u16string Utf8ToUtf16(std::string_view str);

std::u32string Utf8ToUtf32(std::string const& str);

std::u32string Utf8ToUtf32(std::string_view str);

std::string WCharToUtf8(std::wstring const& str);

std::string WCharToUtf8(std::wstring_view str);

std::string Utf16ToUtf8(std::u16string const& str);

std::string Utf16ToUtf8(std::u16string_view str);

std::string Utf32ToUtf8(std::u32string const& str);

std::string Utf32ToUtf8(std::u32string_view str);

// Size of UTF8 chars (not the size of the byte buffer).
size_t EncodedLength(std::string const& str);

size_t EncodedLength(std::string_view str);

namespace Base64
{

std::size_t Encode(void* dest, void const* src, std::size_t len, bool padding);

std::pair<std::size_t, std::size_t> Decode(void* dest, char const* src, std::size_t len);

inline std::size_t constexpr EncodedSize(std::size_t n)
{
    return 4 * ((n + 2) / 3);
}

inline std::size_t constexpr DecodedSize(std::size_t n)
{
    return n * 3 / 4;
}

inline std::string Encode(std::uint8_t const* data, std::size_t len, bool padding)
{
    std::string dest;
    dest.resize(EncodedSize(len));
    dest.resize(Encode(&dest[0], data, len, padding));
    return dest;
}

inline std::string Encode(std::string const& s, bool padding)
{
    return Encode(reinterpret_cast <std::uint8_t const*>(s.data()), s.size(), padding);
}

inline std::string Decode(std::string const& data)
{
    std::string dest;
    dest.resize(DecodedSize(data.size()));
    auto const result = Decode(&dest[0], data.data(), data.size());
    dest.resize(result.first);
    return dest;
}

std::size_t UrlEncode(void* dest, void const* src, std::size_t len, bool padding);

std::pair<std::size_t, std::size_t> UrlDecode(void* dest, char const* src, std::size_t len);

inline std::string UrlEncode(std::uint8_t const* data, std::size_t len, bool padding)
{
    std::string dest;
    dest.resize(EncodedSize(len));
    dest.resize(UrlEncode(&dest[0], data, len, padding));
    return dest;
}

inline std::string UrlEncode(std::string const& s, bool padding)
{
    return UrlEncode(reinterpret_cast <std::uint8_t const*>(s.data()), s.size(), padding);
}

inline std::string UrlDecode(std::string const& data)
{
    std::string dest;
    dest.resize(DecodedSize(data.size()));
    auto const result = UrlDecode(&dest[0], data.data(), data.size());
    dest.resize(result.first);
    return dest;
}

}

}
}
