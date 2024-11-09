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

#define SYSTEM_LOOP_NAME_CAT(x,y) x##y

#if defined(SYSTEM_OS_WINDOWS) && !defined(SYSTEM_COMPILER_CLANG)
#define SYSTEM_LOOP_NAME_DISABLE_WARNING(warningCode) \
    __pragma(warning(push))          \
    __pragma(warning(disable : warningCode))

#define SYSTEM_LOOP_NAME_RESTORE_WARNINGS() \
    __pragma(warning(pop))
#else
#define SYSTEM_LOOP_NAME_DISABLE_WARNING(warningCode)
#define SYSTEM_LOOP_NAME_RESTORE_WARNINGS()
#endif

#define SYSTEM_LOOP_NAME(name) \
	if (constexpr bool _namedloopInvalidBreakOrContinue = false) \
	{ \
		SYSTEM_LOOP_NAME_DISABLE_WARNING(4102) \
		SYSTEM_LOOP_NAME_CAT(_namedloop_break_, name): break; \
		SYSTEM_LOOP_NAME_CAT(_namedloop_continue_, name): continue; \
		SYSTEM_LOOP_NAME_RESTORE_WARNINGS() \
	} \
	else

#define SYSTEM_LOOP_BREAK(name)    goto SYSTEM_LOOP_NAME_CAT(_namedloop_break_, name)
#define SYSTEM_LOOP_CONTINUE(name) goto SYSTEM_LOOP_NAME_CAT(_namedloop_continue_, name)