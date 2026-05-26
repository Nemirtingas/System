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

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace System
{
namespace StringSwitch
{
namespace Detail
{

// =========================
// FNV constants
// =========================
constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ull;
constexpr uint64_t FNV_PRIME        = 1099511628211ull;

// =========================
// Char policies
// =========================
namespace Policy
{
constexpr char identity(char c)
{
    return c;
}

constexpr char lower(char c)
{
    return (c >= 'A' && c <= 'Z') ? (c + 32) : c;
}

constexpr wchar_t identity(wchar_t c)
{
    return c;
}

constexpr wchar_t lower(wchar_t c)
{
    return (c >= L'A' && c <= L'Z') ? (c + 32) : c;
}

} // namespace Policy

// =========================
// Core FNV-1a (generic)
// =========================
template <typename CharT, CharT (*Transform)(CharT)>
constexpr uint64_t fnv1a(const CharT *data, size_t len)
{
    uint64_t hash = FNV_OFFSET_BASIS;

    for (size_t i = 0; i < len; ++i)
    {
        hash ^= static_cast<uint64_t>(Transform(data[i]));
        hash *= FNV_PRIME;
    }

    return hash;
}

template <typename CharT, CharT (*Transform)(CharT)>
constexpr uint64_t fnv1a(const CharT *data)
{
    uint64_t hash = FNV_OFFSET_BASIS;

    while (*data)
    {
        hash ^= static_cast<uint64_t>(Transform(*data++));
        hash *= FNV_PRIME;
    }

    return hash;
}

} // namespace Detail

// =========================
// Public API - case sensitive
// =========================

constexpr uint64_t Hash(const char *s, size_t len)
{
    return Detail::fnv1a<char, Detail::Policy::identity>(s, len);
}

constexpr uint64_t Hash(const char *s)
{
    return Detail::fnv1a<char, Detail::Policy::identity>(s);
}

constexpr uint64_t Hash(std::string_view sv)
{
    return Hash(sv.data(), sv.size());
}

inline uint64_t Hash(const std::string &s)
{
    return Hash(s.data(), s.size());
}

constexpr uint64_t Hash(const wchar_t *s, size_t len)
{
    return Detail::fnv1a<wchar_t, Detail::Policy::identity>(s, len);
}

constexpr uint64_t Hash(const wchar_t *s)
{
    return Detail::fnv1a<wchar_t, Detail::Policy::identity>(s);
}

inline uint64_t Hash(const std::wstring &s)
{
    return Hash(s.data(), s.size());
}

// =========================
// Public API - case insensitive
// =========================

constexpr uint64_t IHash(const char *s, size_t len)
{
    return Detail::fnv1a<char, Detail::Policy::lower>(s, len);
}

constexpr uint64_t IHash(const char *s)
{
    return Detail::fnv1a<char, Detail::Policy::lower>(s);
}

constexpr uint64_t IHash(std::string_view sv)
{
    return IHash(sv.data(), sv.size());
}

inline uint64_t IHash(const std::string &s)
{
    return IHash(s.data(), s.size());
}

constexpr uint64_t IHash(const wchar_t *s, size_t len)
{
    return Detail::fnv1a<wchar_t, Detail::Policy::lower>(s, len);
}

constexpr uint64_t IHash(const wchar_t *s)
{
    return Detail::fnv1a<wchar_t, Detail::Policy::lower>(s);
}

inline uint64_t IHash(const std::wstring &s)
{
    return IHash(s.data(), s.size());
}

} // namespace StringSwitch
} // namespace System
