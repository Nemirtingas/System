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

namespace System {
namespace FunctionName {
namespace Details {
#if !defined(SYSTEM_COMPILER_CLANG) && defined(SYSTEM_OS_WINDOWS)
#define SYSTEM_DETAILS_FUNCTION_NAME __FUNCTION__
constexpr static std::string_view FunctionPrefix{ "" };
#elif defined(SYSTEM_COMPILER_CLANG)
#define SYSTEM_DETAILS_FUNCTION_NAME __PRETTY_FUNCTION__
constexpr static std::string_view FunctionPrefix{ "static const char *" };
#elif defined(SYSTEM_COMPILER_GCC)
#define SYSTEM_DETAILS_FUNCTION_NAME __PRETTY_FUNCTION__
constexpr static std::string_view FunctionPrefix{ "static constexpr const char* " };
#endif


template <std::size_t V, typename T>
struct index_push;

template <std::size_t V, std::size_t... I>
struct index_push<V, std::index_sequence<I...>>
{
    using type = std::index_sequence<V, I...>;
};

template <std::size_t From, std::size_t To>
struct make_range_sequence;

template <std::size_t To>
struct make_range_sequence<To, To>
{
    using type = std::index_sequence<To>;
};

template <std::size_t From, std::size_t To>
using make_range_sequence_t = typename make_range_sequence<From, To>::type;

template <std::size_t From, std::size_t To>
struct make_range_sequence : index_push<From, make_range_sequence_t<From + 1, To>> {};

template<char... CHARS>
struct Chars {
    static constexpr const char value[] = { CHARS..., '\0' };
};
template<char... CHARS>
constexpr const char Chars<CHARS...>::value[];

template<typename WrapperT, typename IDXS>
struct ExtractCharsImpl;

template<typename WrapperT, std::size_t... Indexes>
struct ExtractCharsImpl<WrapperT, std::index_sequence<Indexes...> > {
    using type = Chars<WrapperT::get()[Indexes]...>;
};

template<typename WrapperT, typename SequenceT>
struct ExtractChars {
    using type = typename ExtractCharsImpl<WrapperT, SequenceT> ::type;
};

constexpr inline std::size_t FindFunctionNameStart(const char* str) {
    return FunctionPrefix.length();
}

constexpr inline std::string_view GetFunctionSuffix(const char* str) {
    constexpr std::string_view wellKnownStrings[] = {
        std::string_view { "::(anonymous class)::operator()()::Wrapper::get()" }, // Old Clang
        std::string_view { "::(lambda)::operator()()::Wrapper::get()"          }, // Clang
        std::string_view { "::<lambda()>::Wrapper::get()"                      }, // GCC
        std::string_view { "::()::Wrapper::get"                                }, // MSVC
    };

    std::string_view functionName{ str };

    for (auto wellKnownString : wellKnownStrings) {
        if (functionName.find(wellKnownString) != std::string_view::npos) {
            return wellKnownString;
        }
    }

    return std::string_view{ "" };
}


constexpr inline std::size_t FindFunctionNameStartNamespace(const char* str, const char* ns) {
    auto functionName = std::string_view{ str };
    auto namespaceName = std::string_view{ ns };
    std::size_t startIndex = FindFunctionNameStart(str);
    std::size_t endIndex = functionName.rfind(GetFunctionSuffix(str)) - 1;
    std::size_t nested = 0;

    for (std::size_t index = startIndex; index < endIndex; ++index) {
        char c = str[index];
        if (c == ')' || c == '>') {
            ++nested;
        }
        else if (c == '(' || c == '<') {
            --nested;
        }
        else if (nested == 0) {
            if (functionName.find(namespaceName, index) == index)
            {
                startIndex = index + namespaceName.length();
                break;
            }
        }
    }

    return startIndex;
}

constexpr inline std::size_t FindFunctionNameEnd(const char* str) {
    auto functionName = std::string_view{ str };
    std::size_t startIndex = FindFunctionNameStart(str);
    std::size_t endIndex = functionName.rfind(GetFunctionSuffix(str)) - 1;
    std::size_t nested = 0;

    constexpr std::string_view wellKnownStrings[] = {
        std::string_view{ "::operator ()()" },
        std::string_view{ "::operator()()" },
        std::string_view{ "::operator ()" },
        std::string_view{ "::operator()" },
        std::string_view{ "::(anonymous class)" },
        std::string_view{ "::()" },
    };

    for (std::size_t index = endIndex; index > startIndex;) {
        char c = str[index];
        if (c == ')' || c == '>') {
            if (c == ')') {
                bool leave = false;
                for (auto wellKnownString : wellKnownStrings) {
                    if (functionName.rfind(wellKnownString, index) == (index - wellKnownString.length() + 1)) {
                        index -= wellKnownString.length();
                        leave = true;
                        break;
                    }
                }
                if (leave)
                    continue;
            }
            ++nested;
        }
        else if (c == '(' || c == '<') {
            --nested;
        }
        else if (nested == 0) {
            if (c == ':' && (index - 1 > startIndex) && str[index - 1] == ':') {
                index -= 2;
                continue;
            }
            endIndex = index;
            break;
        }
        --index;
    }

    return endIndex;
}

}//namespace Details
}//namespace FunctionName
}//namespace System

#define SYSTEM_FUNCTION_NAME_IMPL(STR)                                      \
    [](){                                                                   \
        struct Wrapper {                                                    \
            constexpr static const char * get() { return STR; }             \
        };                                                                  \
        return System::FunctionName::Details::ExtractChars<Wrapper, System::FunctionName::Details::make_range_sequence_t<System::FunctionName::Details::FindFunctionNameStart(Wrapper::get()), System::FunctionName::Details::FindFunctionNameEnd(Wrapper::get())>>::type::value; \
    }()

#define SYSTEM_FUNCTION_NAME_NAMESPACE_IMPL(STR, NS)                        \
    [](){                                                                   \
        struct Wrapper {                                                    \
            constexpr static const char * get() { return STR; }             \
        };                                                                  \
        return System::FunctionName::Details::ExtractChars<Wrapper, System::FunctionName::Details::make_range_sequence_t<System::FunctionName::Details::FindFunctionNameStartNamespace(Wrapper::get(), NS), System::FunctionName::Details::FindFunctionNameEnd(Wrapper::get())>>::type::value; \
    }()

//#define SYSTEM_FUNCTION_NAME __FUNCTION__
#define SYSTEM_FUNCTION_NAME               SYSTEM_FUNCTION_NAME_IMPL(SYSTEM_DETAILS_FUNCTION_NAME)
#define SYSTEM_FUNCTION_NAME_NAMESPACE(NS) SYSTEM_FUNCTION_NAME_NAMESPACE_IMPL(SYSTEM_DETAILS_FUNCTION_NAME, NS)
