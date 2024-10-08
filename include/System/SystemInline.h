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

#if defined(_MSC_VER) || \
    defined(__INTEL_COMPILER) || \
    defined(__ARMCC_VERSION) || \
    defined(__ICC) || defined(__ICL) || defined(__ICCARM) || defined(__ICPC)
	#define SYSTEM_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__) || defined(__EMSCRIPTEN__)
	#define SYSTEM_FORCEINLINE __attribute__((always_inline)) inline
#else
	#define SYSTEM_FORCEINLINE inline
#endif