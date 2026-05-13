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
#include <vector>
#include <cstdint>

namespace System
{
namespace Encoding
{
namespace Base64
{

std::size_t Encode(void *dest, void const *src, std::size_t len, bool padding);

/// <summary>
/// Decodes the base64.
/// </summary>
/// <param name="dest">Destination buffer, must be able to hold all the decoded bytes. (Call DecodedSize)</param>
/// <param name="src"></param>
/// <param name="len"></param>
/// <returns>First is the count of written bytes in dest, Second is the count of consumed bytes from src</returns>
std::pair<std::size_t, std::size_t> Decode(void *dest, char const *src, std::size_t len);

inline std::size_t constexpr EncodedSize(std::size_t n)
{
    return 4 * ((n + 2) / 3);
}

inline std::size_t constexpr DecodedSize(std::size_t n)
{
    return n * 3 / 4;
}

inline std::string Encode(std::uint8_t const *data, std::size_t len, bool padding)
{
    std::string dest;
    dest.resize(EncodedSize(len));
    dest.resize(Encode(&dest[0], data, len, padding));
    return dest;
}

inline std::string Encode(std::string_view s, bool padding)
{
    return Encode(reinterpret_cast<std::uint8_t const *>(s.data()), s.size(), padding);
}

inline std::string Decode(std::string_view data)
{
    std::string dest;
    dest.resize(DecodedSize(data.size()));
    auto const result = Decode(&dest[0], data.data(), data.size());
    dest.resize(result.first);
    return dest;
}

std::size_t UrlEncode(void *dest, void const *src, std::size_t len, bool padding);

/// <summary>
///
/// </summary>
/// <param name="dest"></param>
/// <param name="src"></param>
/// <param name="len"></param>
/// <returns>First is the count of written bytes in dest, Second is the count of consumed bytes from src</returns>
std::pair<std::size_t, std::size_t> UrlDecode(void *dest, char const *src, std::size_t len);

inline std::string UrlEncode(std::uint8_t const *data, std::size_t len, bool padding)
{
    std::string dest;
    dest.resize(EncodedSize(len));
    dest.resize(UrlEncode(&dest[0], data, len, padding));
    return dest;
}

inline std::string UrlEncode(std::string_view s, bool padding)
{
    return UrlEncode(reinterpret_cast<std::uint8_t const *>(s.data()), s.size(), padding);
}

inline std::string UrlDecode(std::string_view data)
{
    std::string dest;
    dest.resize(DecodedSize(data.size()));
    auto const result = UrlDecode(&dest[0], data.data(), data.size());
    dest.resize(result.first);
    return dest;
}

} // namespace Base64
} // namespace Encoding
} // namespace System
