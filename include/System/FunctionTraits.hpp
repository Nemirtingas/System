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

#include <System/SystemMacro.h>
#include <System/SystemExports.h>

namespace System {

template<typename Function> struct FunctionTraits;

#define FUNCTION_TRAIT_MEMBER_DECLARE_1(...)                                      typedef R result_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_2(...)  FUNCTION_TRAIT_MEMBER_DECLARE_1() ; typedef T1 arg1_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_3(...)  FUNCTION_TRAIT_MEMBER_DECLARE_2() ; typedef T2 arg2_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_4(...)  FUNCTION_TRAIT_MEMBER_DECLARE_3() ; typedef T3 arg3_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_5(...)  FUNCTION_TRAIT_MEMBER_DECLARE_4() ; typedef T4 arg4_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_6(...)  FUNCTION_TRAIT_MEMBER_DECLARE_5() ; typedef T5 arg5_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_7(...)  FUNCTION_TRAIT_MEMBER_DECLARE_6() ; typedef T6 arg6_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_8(...)  FUNCTION_TRAIT_MEMBER_DECLARE_7() ; typedef T7 arg7_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_9(...)  FUNCTION_TRAIT_MEMBER_DECLARE_8() ; typedef T8 arg8_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_10(...) FUNCTION_TRAIT_MEMBER_DECLARE_9() ; typedef T9 arg9_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_11(...) FUNCTION_TRAIT_MEMBER_DECLARE_10(); typedef T10 arg10_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_12(...) FUNCTION_TRAIT_MEMBER_DECLARE_11(); typedef T11 arg11_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_13(...) FUNCTION_TRAIT_MEMBER_DECLARE_12(); typedef T12 arg12_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_14(...) FUNCTION_TRAIT_MEMBER_DECLARE_13(); typedef T13 arg13_type
#define FUNCTION_TRAIT_MEMBER_DECLARE_15(...) FUNCTION_TRAIT_MEMBER_DECLARE_14(); typedef T14 arg14_type

#define FUNCTION_TRAIT_TYPENAME_DECLARE_1(...)  typename R
#define FUNCTION_TRAIT_TYPENAME_DECLARE_2(...)  FUNCTION_TRAIT_TYPENAME_DECLARE_1() , typename T1
#define FUNCTION_TRAIT_TYPENAME_DECLARE_3(...)  FUNCTION_TRAIT_TYPENAME_DECLARE_2() , typename T2
#define FUNCTION_TRAIT_TYPENAME_DECLARE_4(...)  FUNCTION_TRAIT_TYPENAME_DECLARE_3() , typename T3
#define FUNCTION_TRAIT_TYPENAME_DECLARE_5(...)  FUNCTION_TRAIT_TYPENAME_DECLARE_4() , typename T4
#define FUNCTION_TRAIT_TYPENAME_DECLARE_6(...)  FUNCTION_TRAIT_TYPENAME_DECLARE_5() , typename T5
#define FUNCTION_TRAIT_TYPENAME_DECLARE_7(...)  FUNCTION_TRAIT_TYPENAME_DECLARE_6() , typename T6
#define FUNCTION_TRAIT_TYPENAME_DECLARE_8(...)  FUNCTION_TRAIT_TYPENAME_DECLARE_7() , typename T7
#define FUNCTION_TRAIT_TYPENAME_DECLARE_9(...)  FUNCTION_TRAIT_TYPENAME_DECLARE_8() , typename T8
#define FUNCTION_TRAIT_TYPENAME_DECLARE_10(...) FUNCTION_TRAIT_TYPENAME_DECLARE_9() , typename T9
#define FUNCTION_TRAIT_TYPENAME_DECLARE_11(...) FUNCTION_TRAIT_TYPENAME_DECLARE_10(), typename T10
#define FUNCTION_TRAIT_TYPENAME_DECLARE_12(...) FUNCTION_TRAIT_TYPENAME_DECLARE_11(), typename T11
#define FUNCTION_TRAIT_TYPENAME_DECLARE_13(...) FUNCTION_TRAIT_TYPENAME_DECLARE_12(), typename T12
#define FUNCTION_TRAIT_TYPENAME_DECLARE_14(...) FUNCTION_TRAIT_TYPENAME_DECLARE_13(), typename T13
#define FUNCTION_TRAIT_TYPENAME_DECLARE_15(...) FUNCTION_TRAIT_TYPENAME_DECLARE_14(), typename T14

#define FUNCTION_TRAIT_FUNCTION_TYPE_DECLARE(R, ...) typedef R(*F)(__VA_ARGS__)

#define FUNCTION_TRAIT_DECLARE(R, ...) \
template<SYSTEM_MACRO_CALL_OVERLOAD(FUNCTION_TRAIT_TYPENAME_DECLARE_, R, ##__VA_ARGS__)>\
struct FunctionTraits<R(__VA_ARGS__)>\
{\
	SYSTEM_MACRO_CALL_OVERLOAD(FUNCTION_TRAIT_MEMBER_DECLARE_, R, ##__VA_ARGS__);\
	FUNCTION_TRAIT_FUNCTION_TYPE_DECLARE(R, ##__VA_ARGS__);\
};\
template<SYSTEM_MACRO_CALL_OVERLOAD(FUNCTION_TRAIT_TYPENAME_DECLARE_, R, ##__VA_ARGS__)>\
struct FunctionTraits<R(*)(__VA_ARGS__)>\
{\
	SYSTEM_MACRO_CALL_OVERLOAD(FUNCTION_TRAIT_MEMBER_DECLARE_, R, ##__VA_ARGS__);\
	FUNCTION_TRAIT_FUNCTION_TYPE_DECLARE(R, ##__VA_ARGS__);\
}

FUNCTION_TRAIT_DECLARE(R);
FUNCTION_TRAIT_DECLARE(R, T1);
FUNCTION_TRAIT_DECLARE(R, T1, T2);
FUNCTION_TRAIT_DECLARE(R, T1, T2, T3);
FUNCTION_TRAIT_DECLARE(R, T1, T2, T3, T4);
FUNCTION_TRAIT_DECLARE(R, T1, T2, T3, T4, T5);
FUNCTION_TRAIT_DECLARE(R, T1, T2, T3, T4, T5, T6);
FUNCTION_TRAIT_DECLARE(R, T1, T2, T3, T4, T5, T6, T7);
FUNCTION_TRAIT_DECLARE(R, T1, T2, T3, T4, T5, T6, T7, T8);
FUNCTION_TRAIT_DECLARE(R, T1, T2, T3, T4, T5, T6, T7, T8, T9);
FUNCTION_TRAIT_DECLARE(R, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10);
FUNCTION_TRAIT_DECLARE(R, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11);
FUNCTION_TRAIT_DECLARE(R, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12);
FUNCTION_TRAIT_DECLARE(R, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13);
FUNCTION_TRAIT_DECLARE(R, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14);

}//namespace System