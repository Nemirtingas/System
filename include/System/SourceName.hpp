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

#include <System/SystemDetector.h>
#include <System/SystemCompiler.h>
#include <System/LoopBreak.hpp>
#include <string_view>

#include <System/TemplateStaticStorage.hpp>

namespace System
{
namespace SourceName
{
namespace Details
{
#define SYSTEM_DETAILS_SOURCE_NAME __FILE__

constexpr inline std::size_t FindFileNameStart(const char *str)
{
    std::string_view sourceName{str};

    for (size_t index = sourceName.length(); index > 0; --index)
    {
        char c = sourceName[index - 1];
        if (c == '/' || c == '\\')
            return index;
    }

    return 0;
}

constexpr inline std::size_t FindFileNameEnd(const char *str)
{
    std::string_view sourceName{str};
    return sourceName.length();
}

constexpr inline std::size_t FindDirNameStart(const char *str)
{
    return 0;
}

constexpr inline std::size_t FindDirNameEnd(const char *str)
{
    std::string_view sourceName{str};

    for (size_t index = sourceName.length(); index > 0; --index)
    {
        char c = sourceName[index - 1];
        if (c == '/' || c == '\\')
            return index - 1;
    }

    return 0;
}

constexpr inline std::size_t FindFileExtensionStart(const char *str)
{
    std::string_view sourceName{str};

    auto fileNameStart = FindFileNameStart(str);
    for (size_t index = sourceName.length(); index > fileNameStart; --index)
    {
        char c = sourceName[index - 1];
        if (c == '.')
            return index;
    }

    return 0;
}

constexpr inline std::size_t FindFileExtensionEnd(const char *str)
{
    std::string_view sourceName{str};
    return sourceName.length();
}

constexpr inline std::size_t FindFileRelativeNameStart(const char *rootPath, const char *str)
{
    std::string_view sourceName{str};
    std::string_view rootPathView{rootPath};

    for (size_t index = 0; index < (sourceName.length() - rootPathView.length());)
    {
        auto match = true;
        for (size_t subIndex = 0; subIndex < rootPathView.length(); ++subIndex)
        {
            char sourceC = sourceName[index + subIndex] == '\\' ? '/' : sourceName[index + subIndex];
            char rootC   = rootPathView[subIndex] == '\\' ? '/' : rootPathView[subIndex];

            if (sourceC != rootC)
            {
                index += subIndex;
                match = false;
                break;
            }
        }
        if (match)
            return index + rootPathView.length();
    }

    return 0;
}

} // namespace Details
} // namespace SourceName
} // namespace System

#define SYSTEM_SOURCE_FILE_PATH_IMPL(STR) []() { return STR; }()

#define SYSTEM_SOURCE_FILE_NAME_IMPL(STR)                                                                                                                                          \
    []()                                                                                                                                                                           \
    {                                                                                                                                                                              \
        struct Wrapper                                                                                                                                                             \
        {                                                                                                                                                                          \
            constexpr static const char *get() { return STR; }                                                                                                                     \
        };                                                                                                                                                                         \
        return System::TemplateStaticStorage::Details::ExtractChars<                                                                                                               \
            Wrapper,                                                                                                                                                               \
            System::TemplateStaticStorage::Details::                                                                                                                               \
                make_range_sequence_t<System::SourceName::Details::FindFileNameStart(Wrapper::get()), System::SourceName::Details::FindFileNameEnd(Wrapper::get())>>::type::value; \
    }()

#define SYSTEM_SOURCE_DIR_NAME_IMPL(STR)                                                                                                                                         \
    []()                                                                                                                                                                         \
    {                                                                                                                                                                            \
        struct Wrapper                                                                                                                                                           \
        {                                                                                                                                                                        \
            constexpr static const char *get() { return STR; }                                                                                                                   \
        };                                                                                                                                                                       \
        return System::TemplateStaticStorage::Details::ExtractChars<                                                                                                             \
            Wrapper,                                                                                                                                                             \
            System::TemplateStaticStorage::Details::                                                                                                                             \
                make_range_sequence_t<System::SourceName::Details::FindDirNameStart(Wrapper::get()), System::SourceName::Details::FindDirNameEnd(Wrapper::get())>>::type::value; \
    }()

#define SYSTEM_SOURCE_EXTENSION_NAME_IMPL(STR)                                                    \
    []()                                                                                          \
    {                                                                                             \
        struct Wrapper                                                                            \
        {                                                                                         \
            constexpr static const char *get() { return STR; }                                    \
        };                                                                                        \
        return System::TemplateStaticStorage::Details::ExtractChars<                              \
            Wrapper,                                                                              \
            System::TemplateStaticStorage::Details::make_range_sequence_t<                        \
                System::SourceName::Details::FindFileExtensionStart(Wrapper::get()),              \
                System::SourceName::Details::FindFileExtensionEnd(Wrapper::get())>>::type::value; \
    }()

#define SYSTEM_SOURCE_RELATIVE_FILE_PATH_IMPL(ROOT_PATH, STR)                                      \
    []()                                                                                           \
    {                                                                                              \
        struct Wrapper                                                                             \
        {                                                                                          \
            constexpr static const char *get() { return STR; }                                     \
        };                                                                                         \
        return System::TemplateStaticStorage::Details::ExtractChars<                               \
            Wrapper,                                                                               \
            System::TemplateStaticStorage::Details::make_range_sequence_t<                         \
                System::SourceName::Details::FindFileRelativeNameStart(ROOT_PATH, Wrapper::get()), \
                System::SourceName::Details::FindFileNameEnd(Wrapper::get())>>::type::value;       \
    }()

#define SYSTEM_SOURCE_FILE_PATH SYSTEM_SOURCE_FILE_PATH_IMPL(SYSTEM_DETAILS_SOURCE_NAME)
#define SYSTEM_SOURCE_FILE_NAME SYSTEM_SOURCE_FILE_NAME_IMPL(SYSTEM_DETAILS_SOURCE_NAME)
#define SYSTEM_SOURCE_DIR_NAME SYSTEM_SOURCE_DIR_NAME_IMPL(SYSTEM_DETAILS_SOURCE_NAME)
#define SYSTEM_SOURCE_EXTENSION_NAME SYSTEM_SOURCE_EXTENSION_NAME_IMPL(SYSTEM_DETAILS_SOURCE_NAME)
#define SYSTEM_SOURCE_RELATIVE_FILE_PATH(ROOT_PATH) SYSTEM_SOURCE_RELATIVE_FILE_PATH_IMPL(ROOT_PATH, SYSTEM_DETAILS_SOURCE_NAME)
