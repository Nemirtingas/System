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

#include <string>
#include <vector>
#include <chrono>
#include <string_view>

#ifdef CreateDirectory
#undef CreateDirectory
#endif
#ifdef DeleteFile
#undef DeleteFile
#endif

namespace System {
namespace Filesystem {

    constexpr static char WindowsSeparator = '\\';
    constexpr static char UnixSeparator = '/';

    #if defined(WIN64) || defined(_WIN64) || defined(__MINGW64__) || defined(WIN32) || defined(_WIN32) || defined(__MINGW32__)
        constexpr static char Separator = WindowsSeparator;
    #else
        constexpr static char Separator = UnixSeparator;
    #endif

    std::string CleanPath(std::string const& path);
    std::string Filename(std::string const& path);
    std::string Dirname(std::string const& path);
    bool IsAbsolute(std::string const& path);

    inline std::string Join(std::string_view s) { return std::string{ s }; }
    std::string Join(std::string_view r, std::string_view l);

    template<typename ...Args>
    std::string Join(std::string_view path, Args&& ...args)
    {
        return Join(path, std::string_view(Join(args...)));
    }

    std::string GetCwd();
    std::string CanonicalPath(std::string const& path);

    bool IsDir(std::string const& path);
    bool IsFile(std::string const& path);
    bool Exists(std::string const& path);
    size_t FileSize(std::string const& path);
    std::chrono::system_clock::time_point FileATime(std::string const& path);
    std::chrono::system_clock::time_point FileMTime(std::string const& path);
    std::chrono::system_clock::time_point FileCTime(std::string const& path);

    bool CreateDirectory(std::string const& folder, bool recursive = true);
    bool DeleteFile(std::string const& path);
    std::vector<std::string> ListFiles(std::string const& path, bool files_only, bool recursive = false);

}
}