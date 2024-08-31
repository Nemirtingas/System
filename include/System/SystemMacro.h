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

#define SYSTEM_MACRO_GLUE(x, y) x y

#define SYSTEM_MACRO_RETURN_ARG_COUNT(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, count, ...) count
#define SYSTEM_MACRO_EXPAND_ARGS(vs) SYSTEM_MACRO_RETURN_ARG_COUNT vs
#define SYSTEM_MACRO_COUNT_ARGS_(...) SYSTEM_MACRO_EXPAND_ARGS((__VA_ARGS__, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define SYSTEM_MACRO_OVERLOAD_MACRO__(name, count) name##count
#define SYSTEM_MACRO_OVERLOAD_MACRO_(name, count) SYSTEM_MACRO_OVERLOAD_MACRO__(name, count)
#define SYSTEM_MACRO_OVERLOAD_MACRO(name, count) SYSTEM_MACRO_OVERLOAD_MACRO_(name, count)

#define SYSTEM_MACRO_CALL_OVERLOAD(name, ...) SYSTEM_MACRO_GLUE(SYSTEM_MACRO_OVERLOAD_MACRO(name, SYSTEM_MACRO_COUNT_ARGS_(__VA_ARGS__)), (__VA_ARGS__))