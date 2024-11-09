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

#include "SystemDetector.h"

namespace System {

namespace StringSwitch {

#if defined(SYSTEM_ARCH_X86)
    using hash_type = uint32_t;
#else
    using hash_type = uint64_t;
#endif

namespace Detail {
    constexpr char lower_char(char c) { return ((c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c); }

    constexpr char lower_wchar(wchar_t c) { return ((c >= L'A' && c <= L'Z') ? c + (L'a' - L'A') : c); }

    constexpr size_t strlen(const char* str) { return *str == 0 ? 0 : strlen(str + 1) + 1; }

    constexpr size_t wcslen(const wchar_t* str) { return *str == 0 ? 0 : wcslen(str + 1) + 1; }
}

// switch case on a string
// char
constexpr hash_type Hash(const char* input, size_t len) { return (len > 0 ? static_cast<hash_type>(*input) + 33 * Hash(input + 1, len - 1) : 5381); }

constexpr hash_type Hash(const char* input) { return Hash(input, Detail::strlen(input)); }

inline hash_type Hash(const std::string& input) { return Hash(input.c_str(), input.length()); }

// wchar
constexpr hash_type Hash(const wchar_t* input, size_t len) { return (len > 0 ? static_cast<hash_type>(*input) + 33 * Hash(input + 1, len - 1) : 5381); }

constexpr hash_type Hash(const wchar_t* input) { return Hash(input, Detail::wcslen(input)); }

inline hash_type Hash(const std::wstring& input) { return Hash(input.c_str(), input.length()); }

// custom
constexpr hash_type Hash(std::string_view sv) { return Hash(sv.data(), sv.length()); }

// switch case on a case insensible string
// char
constexpr hash_type IHash(const char* input, size_t len) { return (len > 0 ? static_cast<hash_type>(Detail::lower_char(*input)) + 33 * IHash(input + 1, len - 1) : 5381); }

constexpr inline hash_type IHash(const char *input) { return IHash(input, Detail::strlen(input)); }

inline hash_type IHash(const std::string& input) { return IHash(input.c_str(), input.length()); }

// wchar
constexpr hash_type IHash(const wchar_t* input, size_t len) { return (len > 0 ? static_cast<hash_type>(Detail::lower_wchar(*input)) + 33 * IHash(input + 1, len - 1) : 5381); }

constexpr inline hash_type IHash(const wchar_t* input) { return IHash(input, Detail::wcslen(input)); }

inline hash_type IHash(const std::wstring& input) { return IHash(input.c_str(), input.length()); }

// custom
constexpr hash_type IHash(std::string_view sv) { return IHash(sv.data(), sv.length()); }

}
}
