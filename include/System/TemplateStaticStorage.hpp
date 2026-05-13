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

#include <utility>
#include <cstdint>
#include <string_view>

namespace System
{
namespace TemplateStaticStorage
{
namespace Details
{

template <std::size_t V, typename T> struct index_push;

template <std::size_t V, std::size_t... I> struct index_push<V, std::index_sequence<I...>>
{
    using type = std::index_sequence<V, I...>;
};

template <std::size_t From, std::size_t To> struct make_range_sequence;

template <std::size_t To> struct make_range_sequence<To, To>
{
    using type = std::index_sequence<To>;
};

template <std::size_t From, std::size_t To> using make_range_sequence_t = typename make_range_sequence<From, To>::type;

template <std::size_t From, std::size_t To> struct make_range_sequence : index_push<From, make_range_sequence_t<From + 1, To>>
{
};

template <char... CHARS> struct Chars
{
    static constexpr const char value[] = {CHARS..., '\0'};
};
template <char... CHARS> constexpr const char Chars<CHARS...>::value[];

template <typename WrapperT, typename IDXS> struct ExtractCharsImpl;

template <typename WrapperT, std::size_t... Indexes> struct ExtractCharsImpl<WrapperT, std::index_sequence<Indexes...>>
{
    using type = Chars<WrapperT::get()[Indexes]...>;
};

template <typename WrapperT, typename SequenceT> struct ExtractChars
{
    using type = typename ExtractCharsImpl<WrapperT, SequenceT>::type;
};

} // namespace Details
} // namespace TemplateStaticStorage
} // namespace System
