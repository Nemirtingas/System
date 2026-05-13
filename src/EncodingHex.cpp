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

namespace System
{
namespace Encoding
{
namespace Hex
{

static uint8_t CharToHex(char c)
{
    static constexpr uint8_t hashmap[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // 01234567
        0x08, 0x09, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // 89:;<=>?
        0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, // @ABCDEFG
        // 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // HIJKLMNO
    };

    return hashmap[(static_cast<uint8_t>(c) & 0x1F) ^ 0x10];
}

std::size_t Encode(void *dest, void const *src, std::size_t len, bool upperCase)
{
    static constexpr char upperhexmap[] = "0123456789ABCDEF";
    static constexpr char lowerhexmap[] = "0123456789abcdef";
    const char *hexmap                  = upperCase ? upperhexmap : lowerhexmap;

    char *out          = static_cast<char *>(dest);
    const uint8_t *in  = static_cast<const uint8_t *>(src);
    const uint8_t *end = in + len;

    while (in < end)
    {
        const uint8_t b = *in++;

        *out++ = hexmap[b >> 4];
        *out++ = hexmap[b & 0x0F];
    }

    return out - static_cast<char *>(dest);
}

// NOTE: Works only on the ASCII range. Any other char table will produce an invalid result. (EBCDIC, ...)
std::pair<std::size_t, std::size_t> Decode(void *dest, char const *src, std::size_t len)
{
    if (len & 1)
        return {0, 0};

    int i             = 0;
    uint8_t *out      = static_cast<uint8_t *>(dest);
    const char *in    = src;
    const char *inEnd = in + len;

    while (in < inEnd)
    {
        const uint8_t h = CharToHex(*in++);
        if (h == 0xff)
            break;
        const uint8_t l = CharToHex(*in++);
        if (l == 0xff)
            break;

        *out++ = (h << 4) | l;
    }

    return {out - static_cast<uint8_t *>(dest), in - src};
}

} // namespace Hex
} // namespace Encoding
} // namespace System
