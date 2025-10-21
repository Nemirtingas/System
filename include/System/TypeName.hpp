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
#include <string_view>

namespace System {
namespace TypeName {
namespace Details {
#if !defined(SYSTEM_COMPILER_CLANG) && defined(SYSTEM_OS_WINDOWS)
#define SYSTEM_DETAILS_TYPE_NAME __FUNCSIG__
constexpr static std::string_view FunctionSuffix{ ">(void)" };
constexpr static std::string_view FunctionPrefix{ " System::TypeName::Details::get<" };

// const char *__cdecl System::TypeName::Details::get<int>(void)
// const char *__cdecl System::TypeName::Details::get<float>(void)
// const char *__cdecl System::TypeName::Details::get<char>(void)
// const char *__cdecl System::TypeName::Details::get<const char>(void)
// const char *__cdecl System::TypeName::Details::get<const char*>(void)
// const char *__cdecl System::TypeName::Details::get<int>(void)
// const char *__cdecl System::TypeName::Details::get<double>(void)
// const char *__cdecl System::TypeName::Details::get<class std::basic_string<char, struct std::char_traits<char>, class std::allocator<char> >>(void)
// const char *__cdecl System::TypeName::Details::get<struct Test>(void)
// const char *__cdecl System::TypeName::Details::get<struct Test::InnerTest>(void)
// const char *__cdecl System::TypeName::Details::get<struct Test::InnerTest::InnerInnerTest>(void)
// const char *__cdecl System::TypeName::Details::get<struct TemplateTest<int>>(void)
// const char *__cdecl System::TypeName::Details::get<struct TemplateTest<struct Test::InnerTest>>(void)
// const char *__cdecl System::TypeName::Details::get<struct main::test>(void)
// const char *__cdecl System::TypeName::Details::get<struct main::<lambda_576e07883a81671002d5cb54bc34f8ed>::()::TestLambda>(void)

#elif defined(SYSTEM_COMPILER_CLANG)
#define SYSTEM_DETAILS_TYPE_NAME __PRETTY_FUNCTION__
constexpr static std::string_view FunctionSuffix{ "]" };
constexpr static std::string_view FunctionPrefix{ "System::TypeName::Details::get() [TypeName = " };

// const char *System::TypeName::Details::get() [TypeName = int]
// const char *System::TypeName::Details::get() [TypeName = float]
// const char *System::TypeName::Details::get() [TypeName = char]
// const char *System::TypeName::Details::get() [TypeName = const char]
// const char *System::TypeName::Details::get() [TypeName = const char *]
// const char *System::TypeName::Details::get() [TypeName = int]
// const char *System::TypeName::Details::get() [TypeName = double]
// const char *System::TypeName::Details::get() [TypeName = std::basic_string<char>]
// const char *System::TypeName::Details::get() [TypeName = Test]
// const char *System::TypeName::Details::get() [TypeName = Test::InnerTest]
// const char *System::TypeName::Details::get() [TypeName = Test::InnerTest::InnerInnerTest]
// const char *System::TypeName::Details::get() [TypeName = TemplateTest<int>]
// const char *System::TypeName::Details::get() [TypeName = TemplateTest<Test::InnerTest>]
// const char *System::TypeName::Details::get() [TypeName = test]
// const char *System::TypeName::Details::get() [TypeName = TestLambda]

#elif defined(SYSTEM_COMPILER_GCC)
#define SYSTEM_DETAILS_TYPE_NAME __PRETTY_FUNCTION__
constexpr static std::string_view FunctionSuffix{ "]" };
constexpr static std::string_view FunctionPrefix{ "System::TypeName::Details::get() [with TypeName = " };

// constexpr const char* System::TypeName::Details::get() [with TypeName = int]
// constexpr const char* System::TypeName::Details::get() [with TypeName = float]
// constexpr const char* System::TypeName::Details::get() [with TypeName = char]
// constexpr const char* System::TypeName::Details::get() [with TypeName = const char]
// constexpr const char* System::TypeName::Details::get() [with TypeName = const char*]
// constexpr const char* System::TypeName::Details::get() [with TypeName = int]
// constexpr const char* System::TypeName::Details::get() [with TypeName = double]
// constexpr const char* System::TypeName::Details::get() [with TypeName = std::__cxx11::basic_string<char>]
// constexpr const char* System::TypeName::Details::get() [with TypeName = Test]
// constexpr const char* System::TypeName::Details::get() [with TypeName = Test::InnerTest]
// constexpr const char* System::TypeName::Details::get() [with TypeName = Test::InnerTest::InnerInnerTest]
// constexpr const char* System::TypeName::Details::get() [with TypeName = TemplateTest<int>]
// constexpr const char* System::TypeName::Details::get() [with TypeName = TemplateTest<Test::InnerTest>]
// constexpr const char* System::TypeName::Details::get() [with TypeName = main(int, char**)::test]
// constexpr const char* System::TypeName::Details::get() [with TypeName = main(int, char**)::<lambda()>::TestLambda]

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

constexpr inline std::size_t FindTypeNameStart(const char* str) {
    auto functionName = std::string_view{ str };
    auto pos = functionName.find(FunctionPrefix);
    if (pos == std::string_view::npos)
        return pos;

    constexpr std::string_view structPrefix{"struct "};
    constexpr std::string_view classPrefix{"class "};
    constexpr std::string_view enumPrefix{"enum "};
    constexpr std::string_view unionPrefix{"union "};

    auto structPos = functionName.find(structPrefix, pos);
    if (structPos == (pos + FunctionPrefix.length()))
        pos += structPrefix.length();
    else
    {
        structPos = functionName.find(classPrefix, pos);
        if (structPos == (pos + FunctionPrefix.length()))
            pos += classPrefix.length();
        else
        {
            structPos = functionName.find(enumPrefix, pos);
            if (structPos == (pos + FunctionPrefix.length()))
                pos += enumPrefix.length();
            else
            {
                structPos = functionName.find(unionPrefix, pos);
                if (structPos == (pos + FunctionPrefix.length()))
                    pos += unionPrefix.length();
            }
        }
    }

    return pos + FunctionPrefix.length();
}

constexpr inline std::size_t FindTypeNameEnd(const char* str) {
    auto functionName = std::string_view{ str };
    std::size_t startIndex = FindTypeNameStart(str);
    std::size_t endIndex = functionName.rfind(FunctionSuffix) - 1;
    std::size_t nested = 0;

    for (std::size_t index = endIndex; index > startIndex;) {
        char c = str[index];
        if (c == ')' || c == '>') {
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

template<typename TypeName>
constexpr static const char* get() { return SYSTEM_DETAILS_TYPE_NAME; }

}//namespace Details

template<typename Name>
constexpr const char* GetTypeName()
{
    struct Wrapper {
        static constexpr const char* get() { return Details::get<Name>(); }
    };

    //return Wrapper::get();
    return System::TypeName::Details::ExtractChars<
        Wrapper,
        System::TypeName::Details::make_range_sequence_t<
            System::TypeName::Details::FindTypeNameStart(Wrapper::get()),
            System::TypeName::Details::FindTypeNameEnd(Wrapper::get())
        >
    >::type::value;
}

}//namespace TypeName
}//namespace System

