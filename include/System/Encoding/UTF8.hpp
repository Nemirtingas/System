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

namespace System
{
namespace Encoding
{
namespace UTF8
{

std::wstring Utf8ToWChar(std::string const &str);

std::wstring Utf8ToWChar(std::string_view str);

std::u16string Utf8ToUtf16(std::string const &str);

std::u16string Utf8ToUtf16(std::string_view str);

std::u32string Utf8ToUtf32(std::string const &str);

std::u32string Utf8ToUtf32(std::string_view str);

std::string WCharToUtf8(std::wstring const &str);

std::string WCharToUtf8(std::wstring_view str);

std::string Utf16ToUtf8(std::u16string const &str);

std::string Utf16ToUtf8(std::u16string_view str);

std::string Utf32ToUtf8(std::u32string const &str);

std::string Utf32ToUtf8(std::u32string_view str);

} // namespace UTF8
} // namespace Encoding
} // namespace System
