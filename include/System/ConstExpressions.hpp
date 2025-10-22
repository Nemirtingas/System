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

#include <cstddef> // size_t

namespace System {
namespace ConstExpr {
    template<typename T, size_t N>
    constexpr size_t CountOf(T(&)[N])
    {
        return N;
    }

    template<typename T, size_t N>
    constexpr size_t StrLen(T(&)[N])
    {
        return N-1;
    }

    // Sum of all parameter pack sizeof
    template <typename... Ts>
    constexpr size_t size_of = 0;

    template <typename T, typename... Ts>
    constexpr size_t size_of<T, Ts...> = sizeof(T) + size_of<Ts...>;

    //// ---------------- Char pack ----------------
    //template<char... CHARS>
    //struct Chars {
    //    static constexpr const char value[] = {CHARS...};
    //};
    //template<char... CHARS>
    //constexpr const char Chars<CHARS...>::value[];
    //
    //// ---------------- Extract chars from literal ----------------
    //template<typename WrapperT, typename IDXS>
    //struct ExtractCharsImpl;
    //
    //template<typename WrapperT, std::size_t... Indexes>
    //struct ExtractCharsImpl<WrapperT, std::index_sequence<Indexes...>> {
    //    using type = Chars<WrapperT::get()[Indexes]...>;
    //};
    //
    //template<typename WrapperT, typename SequenceT>
    //struct ExtractChars {
    //    using type = typename ExtractCharsImpl<WrapperT, SequenceT>::type;
    //};
    //
    //// ---------------- String length constexpr ----------------
    //constexpr inline std::size_t StringLength(const char* str) {
    //    std::size_t l = 0;
    //    while (str[l] != 0) ++l;
    //    return l;
    //}
    //
    //// ---------------- Convert literal to char_sequence ----------------
    //#define STRING_LITERAL_TO_PACK(STR) \
    //    [](){ \
    //        struct Wrapper { \
    //            constexpr static const char * get() { return STR; } \
    //        }; \
    //        return ExtractChars<Wrapper, std::make_index_sequence<StringLength(Wrapper::get())>>::type::value; \
    //    }()
    //
    //// ---------------- Whitelist ----------------
    //template<char... Allowed>
    //struct Whitelist {
    //    template<char C>
    //    constexpr static bool contains = ((C == Allowed) || ...);
    //};
    //
    //// ---------------- char_sequence compatible ----------------
    //template<char... Cs>
    //struct char_sequence {
    //    constexpr static std::array<char, sizeof...(Cs)> value = { Cs... };
    //    template<char C>
    //    using push_back = char_sequence<Cs..., C>;
    //};
    //template<char... Cs>
    //constexpr std::array<char, sizeof...(Cs)> char_sequence<Cs...>::value;
    //
    //// ---------------- Filter char pack ----------------
    //template<typename Seq, typename WL, char... Cs>
    //struct FilterWhitelistCharsImpl;
    //
    //template<typename Seq, typename WL>
    //struct FilterWhitelistCharsImpl<Seq, WL> {
    //    using type = Seq;
    //};
    //
    //template<typename Seq, typename WL, char C, char ...Cs>
    //struct FilterWhitelistCharsImpl<Seq, WL, C, Cs...> {
    //    using NewSeq = std::conditional_t<
    //        WL::template contains<C>,
    //        typename Seq::template push_back<C>,
    //        Seq
    //    >;
    //    using type = typename FilterWhitelistCharsImpl<NewSeq, WL, Cs...>::type;
    //};
    //
    //template<typename Seq, typename WL, typename WrapperT, typename IDXS>
    //struct FilterWhitelistImpl;
    //
    //template<typename Seq, typename WL, typename WrapperT, std::size_t... Indexes>
    //struct FilterWhitelistImpl<Seq, WL, WrapperT, std::index_sequence<Indexes...>> {
    //    using type = typename FilterWhitelistCharsImpl<Seq, WL, WrapperT::get()[Indexes]...>::type;
    //};
    //
    //template<typename WL, typename WrapperT, typename SequenceT>
    //struct FilterWhitelist {
    //    using type = typename FilterWhitelistImpl<char_sequence<>, WL, WrapperT, SequenceT>::type;
    //};
    //
    //
    //
    //
    //template<uint8_t... Cs>
    //struct byte_sequence {
    //    constexpr static std::array<uint8_t, sizeof...(Cs)> value = { Cs... };
    //    template<uint8_t C>
    //    using push_back = byte_sequence<Cs..., C>;
    //};
    //template<uint8_t... Cs>
    //constexpr std::array<uint8_t, sizeof...(Cs)> byte_sequence<Cs...>::value;
    //
    //template<char... Allowed>
    //struct CanTransform {
    //    template<char C>
    //    constexpr static bool can_transform = ((C == Allowed) || ...);
    //};
    //
    //struct HexCharToByteTransformer : public CanTransform<
    //    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    //    'a', 'b', 'c', 'd', 'e', 'f',
    //    'A', 'B', 'C', 'D', 'E', 'F',
    //    '?'> {
    //
    //    constexpr static uint8_t transform(char c)
    //    {
    //        // Works but ASCII dependant
    //        //static constexpr uint8_t hashmap[] =
    //        //{
    //        //  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    //        //  0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //        //  0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00,
    //        //};
    //        //return hashmap[(c & 0x1F) ^ 0x10];
    //        if (c >= '0' && c <= '9') return c - '0';
    //        if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    //        if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    //        return 0x00;
    //    }
    //};
    //
    //struct HexCharToMaskTransformer : public CanTransform<
    //    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    //    'a', 'b', 'c', 'd', 'e', 'f',
    //    'A', 'B', 'C', 'D', 'E', 'F',
    //    '?'> {
    //
    //    constexpr static uint8_t transform(char c)
    //    {
    //        if (c == '?') return 0x00;
    //        return 0x0f;
    //    }
    //};
    //
    //template<typename Seq, typename TRANSFORMER, char... Cs>
    //struct TransformCharsImpl;
    //
    //template<typename Seq, typename TRANSFORMER>
    //struct TransformCharsImpl<Seq, TRANSFORMER> {
    //    using type = Seq;
    //};
    //
    //template<typename Seq, typename TRANSFORMER, char C, char ...Cs>
    //struct TransformCharsImpl<Seq, TRANSFORMER, C, Cs...> {
    //    using NewSeq = std::conditional_t<
    //        TRANSFORMER::template can_transform<C>,
    //        typename Seq::template push_back<TRANSFORMER::transform(C)>,
    //        Seq
    //    >;
    //    using type = typename TransformCharsImpl<NewSeq, TRANSFORMER, Cs...>::type;
    //};
    //
    //template<typename Seq, typename TRANSFORMER, typename WrapperT, typename IDXS>
    //struct TransformImpl;
    //
    //template<typename Seq, typename TRANSFORMER, typename WrapperT, std::size_t... Indexes>
    //struct TransformImpl<Seq, TRANSFORMER, WrapperT, std::index_sequence<Indexes...>> {
    //    using type = typename TransformCharsImpl<Seq, TRANSFORMER, WrapperT::get()[Indexes]...>::type;
    //};
    //
    //template<typename TRANSFORMER, typename WrapperT, typename SequenceT>
    //struct Transform {
    //    using type = typename TransformImpl<byte_sequence<>, TRANSFORMER, WrapperT, SequenceT>::type;
    //};
    //
    //template<typename Seq>
    //struct CombineNibbles;
    //
    //template<uint8_t... Ns>
    //struct CombineNibbles<byte_sequence<Ns...>> {
    //private:
    //    static constexpr uint8_t src[] = { Ns... };
    //    static constexpr std::size_t N = sizeof...(Ns) / 2;
    //
    //    template<std::size_t... I>
    //    static constexpr auto combine(std::index_sequence<I...>) {
    //        return byte_sequence<
    //            ((src[2 * I] << 4) | src[2 * I + 1])...
    //        >{};
    //    }
    //
    //public:
    //    using type = decltype(combine(std::make_index_sequence<N>{}));
    //};
    //
    //// ---------------- Helper filter_literal ----------------
    //#define CONSTEXPR_FILTER_STRING_LITERAL(WL, STR) \
    //    [](){ \
    //        struct Wrapper { \
    //            constexpr static const char * get() { return STR; } \
    //            constexpr static size_t size() { return StringLength(STR); } \
    //        }; \
    //        return FilterWhitelist<WL, Wrapper, std::make_index_sequence<Wrapper::size()>>::type::value; \
    //    }()
    //
    //#define CONSTEXPR_TRANSFORM_STRING_LITERAL(TRANSFORMER, STR) \
    //    [](){ \
    //        struct Wrapper { \
    //            constexpr static const char * get() { return STR; } \
    //            constexpr static size_t size() { return StringLength(STR); } \
    //        }; \
    //        return Transform<TRANSFORMER, Wrapper, std::make_index_sequence<Wrapper::size()>>::type::value; \
    //    }()
    //
    //#define CONSTEXPR_TRANSFORM_HEX_STRING_LITERAL_TO_BYTES(TRANSFORMER, STR) \
    //    [](){ \
    //        struct Wrapper { \
    //            constexpr static const char * get() { return STR; } \
    //            constexpr static size_t size() { return StringLength(STR); } \
    //        }; \
    //        \
    //        using Nibbles = typename Transform< \
    //            TRANSFORMER, \
    //            Wrapper, \
    //            std::make_index_sequence<StringLength(Wrapper::get())> \
    //        >::type; \
    //        \
    //        using Bytes = typename CombineNibbles<Nibbles>::type; \
    //        \
    //        return Bytes::value; \
    //    }()
    //
    //template<typename Transformer>
    //constexpr auto transform_hex_string_to_bytes(const char* str)
    //{
    //     struct Wrapper {
    //            constexpr static const char * get() { return str; }
    //            constexpr static size_t size() { return StringLength(str); }
    //    };
    //
    //    using Nibbles = typename Transform<Transformer, Wrapper, std::make_index_sequence<StringLength(Wrapper::get())>>::type;
    //    using Bytes = typename CombineNibbles<Nibbles>::type;
    //    return Bytes::value;
    //}
    //
    //template<size_t N>
    //struct Pattern_t
    //{
    //    std::array<uint8_t, N> pattern;
    //    std::array<uint8_t, N> mask;
    //    bool hasWildcard;
    //};
    //
    //#define CONSTEXPR_PATTERN_PARSE(STR) \
    //    [](){\
    //        constexpr auto b = CONSTEXPR_TRANSFORM_HEX_STRING_LITERAL_TO_BYTES(HexCharToByteTransformer, STR); \
    //        constexpr auto m = CONSTEXPR_TRANSFORM_HEX_STRING_LITERAL_TO_BYTES(HexCharToMaskTransformer, STR); \
    //        bool w = false; \
    //        for (size_t i = 0; i < m.size(); ++i) if (m[i] != 0xFF) { w = true; break; }\
    //        return Pattern_t<b.size()> { \
    //            b, \
    //            m, \
    //            w \
    //        }; \
    //    }()
    //
    //template<char... CHARS>
    //struct CharArray {
    //    static constexpr std::array<char, sizeof...(CHARS)> value = { CHARS... };
    //};
    //template<char... CHARS>
    //constexpr std::array<char, sizeof...(CHARS)> CharArray<CHARS...>::value;
    //
    //template<typename WrapperT, typename IDXS>
    //struct MakeLiteralStringImpl;
    //
    //template<typename WrapperT, std::size_t... Indexes>
    //struct MakeLiteralStringImpl<WrapperT, std::index_sequence<Indexes...>> {
    //    using type = typename CharArray<WrapperT::get()[Indexes]...>;
    //};
    //
    //template<typename WrapperT, typename Sequence>
    //struct MetaLiteralString {
    //    using type = typename MakeLiteralStringImpl<WrapperT, Sequence>::type;
    //};
    //
    //#define CONSTEXPR_LITERAL_STRING(STR) \
    //    [](){\
    //        struct LiteralStringWrapper { \
    //            constexpr static const char* get() { return STR; } \
    //        }; \
    //        return MetaLiteralString<LiteralStringWrapper, std::make_index_sequence<StringLength(STR)>>::type::value; \
    //    }()

}
}
