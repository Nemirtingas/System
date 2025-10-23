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
#include <utility>

namespace System {
namespace ConstExpr {

    namespace Details {
        // Pushes an index into an std::index_sequence
        template <std::size_t V, typename T>
        struct index_push;

        template <std::size_t V, std::size_t... I>
        struct index_push<V, std::index_sequence<I...>>
        {
            using type = std::index_sequence<V, I...>;
        };

        // Creates an std::sequence from [start,end] instead of [0,end]
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

        // Creates a new static storage for a set of chars
        template<char... CHARS>
        struct Chars {
            static constexpr const char value[] = { CHARS... };
        };
        template<char... CHARS>
        constexpr const char Chars<CHARS...>::value[];

        // Creates a new static storage for a set of chars, '\0' is added at the end for c-string compatibility
        template<char... CHARS>
        struct NullTerminatedChars {
            static constexpr const char value[] = { CHARS..., '\0' };
        };
        template<char... CHARS>
        constexpr const char NullTerminatedChars<CHARS...>::value[];

        // ---------------- char_sequence compatible ----------------
        template<char... Cs>
        struct NullTerminatedCharsSequence {
            constexpr static const char value[] = { Cs..., '\0' };
            template<char C>
            using push_back = NullTerminatedCharsSequence<Cs..., C>;
        };
        template<char... Cs>
        constexpr const char NullTerminatedCharsSequence<Cs...>::value[];

        template<typename WrapperT, typename IDXS>
        struct ExtractNullTerminatedCharsImpl;

        template<typename WrapperT, std::size_t... Indexes>
        struct ExtractNullTerminatedCharsImpl<WrapperT, std::index_sequence<Indexes...>> {
            using type = NullTerminatedChars<WrapperT::get()[Indexes]...>;
        };

        template<typename WrapperT, typename IDXS>
        struct ExtractCharsImpl;

        template<typename WrapperT, std::size_t... Indexes>
        struct ExtractCharsImpl<WrapperT, std::index_sequence<Indexes...>> {
            using type = Chars<WrapperT::get()[Indexes]...>;
        };

        template<typename Seq, typename TransformerT, uint8_t... Bs>
        struct TransformBytesImpl;

        template<typename Seq, typename TransformerT>
        struct TransformBytesImpl<Seq, TransformerT> {
            using type = Seq;
        };

        template<typename Seq, typename TransformerT, uint8_t B, uint8_t ...Bs>
        struct TransformBytesImpl<Seq, TransformerT, B, Bs...> {
            using NewSeq = std::conditional_t<
                TransformerT::template can_transform<B>,
                typename Seq::template push_back<TransformerT::transform(B)>,
                Seq
            >;
            using type = typename TransformBytesImpl<NewSeq, TransformerT, Bs...>::type;
        };

        template<typename Seq, typename TransformerT, typename WrapperT, typename IDXS>
        struct TransformImpl;

        template<typename Seq, typename TransformerT, typename WrapperT, std::size_t... Indexes>
        struct TransformImpl<Seq, TransformerT, WrapperT, std::index_sequence<Indexes...>> {
            using type = typename TransformBytesImpl<Seq, TransformerT, WrapperT::get()[Indexes]...>::type;
        };

        template<typename Seq, typename WL, char... Cs>
        struct StringFilterWhitelistCharsImpl;

        template<typename Seq, typename WL>
        struct StringFilterWhitelistCharsImpl<Seq, WL> {
            using type = Seq;
        };

        template<typename Seq, typename WL, char C, char ...Cs>
        struct StringFilterWhitelistCharsImpl<Seq, WL, C, Cs...> {
            using NewSeq = std::conditional_t<
                WL::template contains<C>,
                typename Seq::template push_back<C>,
                Seq
            >;
            using type = typename StringFilterWhitelistCharsImpl<NewSeq, WL, Cs...>::type;
        };

        template<typename Seq, typename WL, typename WrapperT, typename IDXS>
        struct StringFilterWhitelistImpl;

        template<typename Seq, typename WL, typename WrapperT, std::size_t... Indexes>
        struct StringFilterWhitelistImpl<Seq, WL, WrapperT, std::index_sequence<Indexes...>> {
            using type = typename StringFilterWhitelistCharsImpl<Seq, WL, WrapperT::get()[Indexes]...>::type;
        };

    }// namespace Details

    template<typename T, size_t N>
    constexpr size_t CountOf(T(&)[N])
    {
        return N;
    }

    template<typename T, size_t N>
    constexpr inline size_t StrLen(T(&)[N])
    {
        return N-1;
    }

    constexpr inline std::size_t StrLenUntilNull(const char* str)
    {
        std::size_t l = 0;
        while (str[l] != 0) ++l;
        return l;
    }

    // Sum of all parameter pack sizeof
    template <typename... Ts>
    constexpr size_t size_of = 0;

    template <typename T, typename... Ts>
    constexpr size_t size_of<T, Ts...> = sizeof(T) + size_of<Ts...>;

    // Extracts characters from a sequence and allocate a new storage for them
    template<typename WrapperT, typename SequenceT>
    struct ExtractChars {
        using type = typename Details::ExtractCharsImpl<WrapperT, SequenceT>::type;
    };

    template<typename WrapperT, typename SequenceT>
    struct ExtractNullTerminatedChars {
        using type = typename Details::ExtractNullTerminatedCharsImpl<WrapperT, SequenceT>::type;
    };
    
    // ---------------- Whitelist ----------------
    template<char... Allowed>
    struct Whitelist {
        template<char C>
        constexpr static bool contains = ((C == Allowed) || ...);
    };
    
    // ---------------- Filter char pack ----------------
    
    template<typename WL, typename WrapperT>
    struct StringFilterWhitelist {
        using type = typename Details::StringFilterWhitelistImpl<Details::NullTerminatedCharsSequence<>, WL, WrapperT, std::make_index_sequence<WrapperT::size()>>::type;
    };

    template<typename WL, typename WrapperT>
    using StringFilterWhitelist_t = typename StringFilterWhitelist<WL, WrapperT>::type;
    
    template<uint8_t... Cs>
    struct byte_sequence {
        constexpr static std::array<uint8_t, sizeof...(Cs)> value = { Cs... };
        template<uint8_t C>
        using push_back = byte_sequence<Cs..., C>;
    };
    template<uint8_t... Cs>
    constexpr std::array<uint8_t, sizeof...(Cs)> byte_sequence<Cs...>::value;
    
    template<char... Allowed>
    struct CanTransform {
        template<char C>
        constexpr static bool can_transform = ((C == Allowed) || ...);
    };
    
    struct HexCharToByteTransformer : public CanTransform<
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f',
        'A', 'B', 'C', 'D', 'E', 'F',
        '?'> {
    
        constexpr static uint8_t transform(char c)
        {
            // Works but ASCII dependant
            //static constexpr uint8_t hashmap[] =
            //{
            //  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            //  0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            //  0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00,
            //};
            //return hashmap[(c & 0x1F) ^ 0x10];
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
            if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
            return 0x00;
        }
    };
    
    struct HexCharToMaskTransformer : public CanTransform<
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f',
        'A', 'B', 'C', 'D', 'E', 'F',
        '?'> {
    
        constexpr static uint8_t transform(char c)
        {
            if (c == '?') return 0x00;
            return 0x0f;
        }
    };
    
    template<typename TransformerT, typename WrapperT, typename SequenceT>
    struct Transform {
        using type = typename Details::TransformImpl<byte_sequence<>, TransformerT, WrapperT, SequenceT>::type;
    };

    template<typename TransformerT, typename WrapperT, typename SequenceT>
    using Transform_t = typename Transform<TransformerT, WrapperT, SequenceT>::type;
    
    template<typename Seq>
    struct CombineNibbles;
    
    template<uint8_t... Ns>
    struct CombineNibbles<byte_sequence<Ns...>> {
    private:
        static constexpr uint8_t src[] = { Ns... };
        static constexpr std::size_t N = sizeof...(Ns) / 2;
    
        template<std::size_t... I>
        static constexpr auto combine(std::index_sequence<I...>) {
            return byte_sequence<
                ((src[2 * I] << 4) | src[2 * I + 1])...
            >{};
        }
    
    public:
        using type = decltype(combine(std::make_index_sequence<N>{}));
    };

    template<typename Seq>
    using CombineNibbles_t = typename CombineNibbles<Seq>::type;

    template<typename WrapperT>
    using StringToBytes_t = Transform_t<HexCharToByteTransformer, WrapperT, std::make_index_sequence<WrapperT::size()>>;

    template<typename WrapperT>
    using HexStringToBytes_t = CombineNibbles_t<StringToBytes_t<WrapperT>>;

    #define SYSTEM_MAKE_STRING_LITERAL(STR)                                                    \
        struct {                                                                               \
            constexpr static const char * get() { return STR; }                                \
            constexpr static size_t size() { return System::ConstExpr::StrLenUntilNull(STR); } \
        }
}
}
