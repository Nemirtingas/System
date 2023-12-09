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

#include <System/SystemDetector.h>
#include <System/Guid.hpp>

#if defined(SYSTEM_OS_WINDOWS)
    #define SSCANF(...) sscanf_s(__VA_ARGS__)
#else
    #define SSCANF(...) sscanf(__VA_ARGS__)
#endif

namespace {

constexpr const char GUID_LOWER_STRING_FORMAT[] = "%08x-%04hx-%04hx-%04hx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx";
constexpr const char GUID_UPPER_STRING_FORMAT[] = "%08X-%04hX-%04hX-%04hX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX";

static bool IsValidGUID(std::string const& guidString)
{
    if (guidString.length() != 36
        || guidString[8] != '-' || guidString[13] != '-'
        || guidString[18] != '-' || guidString[23] != '-')
    {
        return false;
    }

    for (int i = 0; i < 36; ++i)
    {
        if (i != 8 && i != 13 && i != 18 && i != 23)
        {
            char c = guidString[i];
            if (((c < '0') || (c > '9')) &&
                ((c < 'A') || (c > 'F')) &&
                ((c < 'a') || (c > 'f')))
            {
                return false;
            }
        }
    }
    return true;
}

}

namespace System {

bool Guid::FromString(std::string const& stringGuid)
{
    if (!IsValidGUID(stringGuid))
        return false;

    SSCANF(stringGuid.c_str(), GUID_LOWER_STRING_FORMAT,
        reinterpret_cast<uint32_t*>(&_Data.Bytes[0]),
        reinterpret_cast<uint16_t*>(&_Data.Bytes[4]),
        reinterpret_cast<uint16_t*>(&_Data.Bytes[6]),
        reinterpret_cast<uint16_t*>(&_Data.Bytes[8]),
        reinterpret_cast<uint8_t*>(&_Data.Bytes[10]),
        reinterpret_cast<uint8_t*>(&_Data.Bytes[11]),
        reinterpret_cast<uint8_t*>(&_Data.Bytes[12]),
        reinterpret_cast<uint8_t*>(&_Data.Bytes[13]),
        reinterpret_cast<uint8_t*>(&_Data.Bytes[14]),
        reinterpret_cast<uint8_t*>(&_Data.Bytes[15]));

	return true;
}

std::string Guid::ToString(bool upperCaseHex) const
{
    std::string res(36, '\0');

    snprintf(&res[0], 37, upperCaseHex ? GUID_UPPER_STRING_FORMAT : GUID_LOWER_STRING_FORMAT,
        *reinterpret_cast<const uint32_t*>(&_Data.Bytes[0]),
        *reinterpret_cast<const uint16_t*>(&_Data.Bytes[4]),
        *reinterpret_cast<const uint16_t*>(&_Data.Bytes[6]),
        *reinterpret_cast<const uint16_t*>(&_Data.Bytes[8]),
        *reinterpret_cast<const uint8_t*>(&_Data.Bytes[10]),
        *reinterpret_cast<const uint8_t*>(&_Data.Bytes[11]),
        *reinterpret_cast<const uint8_t*>(&_Data.Bytes[12]),
        *reinterpret_cast<const uint8_t*>(&_Data.Bytes[13]),
        *reinterpret_cast<const uint8_t*>(&_Data.Bytes[14]),
        *reinterpret_cast<const uint8_t*>(&_Data.Bytes[15]));

    return res;
}

}