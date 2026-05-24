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
namespace Hex
{
std::size_t Encode(void *dest, void const *src, std::size_t len, bool upperCase);

/// <summary>
///
/// </summary>
/// <param name="dest"></param>
/// <param name="src"></param>
/// <param name="len"></param>
/// <returns>First is the count of written bytes in dest, Second is the count of consumed bytes from src</returns>
std::pair<std::size_t, std::size_t> Decode(void *dest, char const *src, std::size_t len);

inline std::size_t constexpr EncodedSize(std::size_t n)
{
    return n * 2;
}

inline std::size_t constexpr DecodedSize(std::size_t n)
{
    return (n + 1) / 2;
}

inline std::string Encode(std::uint8_t const *data, std::size_t len, bool upperCase)
{
    std::string dest;
    dest.resize(EncodedSize(len));
    dest.resize(Encode(dest.data(), data, len, upperCase));
    return dest;
}

inline std::vector<uint8_t> Decode(std::string_view data)
{
    std::vector<uint8_t> dest;
    dest.resize(DecodedSize(data.size()));
    auto const result = Decode(dest.data(), data.data(), data.size());
    return dest;
}

} // namespace Hex
} // namespace Encoding
} // namespace System
