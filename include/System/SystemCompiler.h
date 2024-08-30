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

#if defined(__clang__)
    #define SYSTEM_COMPILER_CLANG
#endif
#if defined(_MSC_VER)
    #define SYSTEM_COMPILER_MSVC
#endif
#if defined(__MINGW32__) || defined(__MINGW64__)
    #define SYSTEM_COMPILER_MINGW
#endif
#if defined(__apple_build_version__)
    #define SYSTEM_COMPILER_APPLE
#endif
#if defined(__GNUC__)
    #define SYSTEM_COMPILER_GCC
#endif
#if defined(__INTEL_COMPILER) || defined(__ICL)
    #define SYSTEM_COMPILER_INTEL
#endif
#if defined(__EMSCRIPTEN__)
    #define SYSTEM_COMPILER_EMSCRIPTEN
#endif
#if defined(__ARMCC_VERSION)
    #define SYSTEM_COMPILER_ARMCC
#endif
#if defined(__BORLANDC__)
    #define SYSTEM_COMPILER_BORLAND
#endif
#if defined(__IBMC__) || defined(__IBMCPP__)
    #define SYSTEM_COMPILER_IBM
#endif
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
    #define SYSTEM_COMPILER_SOLARIS
#endif
#if defined(__hpux) || defined(__HP_aCC)
    #define SYSTEM_COMPILER_HPUX
#endif

#ifdef __cplusplus
#include <array>

 // Some constexpr for C++17 constexpr if.
namespace System {
    enum class Compiler {
        Unknown    = 0,
        MSVC       = 1 << 0,
        MinGW      = 1 << 1,
        Clang      = 1 << 2,
        GCC        = 1 << 3,
        Apple      = 1 << 4,
        Intel      = 1 << 5,
        Emscripten = 1 << 6,
        ARMCC      = 1 << 7,
        Borland    = 1 << 8,
        IBM        = 1 << 9,
        Solaris    = 1 << 10,
        HPUX       = 1 << 11,
    };

    static constexpr Compiler compiler = (Compiler)((int)Compiler::Unknown
#if defined(SYSTEM_COMPILER_MSVC)
        | (int)Compiler::MSVC
#endif
#if defined(SYSTEM_COMPILER_MINGW)
        | (int)Compiler::MinGW
#endif
#if defined(SYSTEM_COMPILER_CLANG)
        | (int)Compiler::Clang
#endif
#if defined(SYSTEM_COMPILER_GCC)
        | (int)Compiler::GCC
#endif
#if defined(SYSTEM_COMPILER_APPLE)
        | (int)Compiler::Apple
#endif
#if defined(SYSTEM_COMPILER_INTEL)
        | (int)Compiler::Intel
#endif
#if defined(SYSTEM_COMPILER_EMSCRIPTEN)
        | (int)Compiler::Emscripten
#endif
#if defined(SYSTEM_COMPILER_ARMCC)
        | (int)Compiler::ARMCC
#endif
#if defined(SYSTEM_COMPILER_BORLAND)
        | (int)Compiler::Borland
#endif
#if defined(SYSTEM_COMPILER_IBM)
        | (int)Compiler::IBM
#endif
#if defined(SYSTEM_COMPILER_SOLARIS)
        | (int)Compiler::Solaris
#endif
#if defined(SYSTEM_COMPILER_HPUX)
        | (int)Compiler::HPUX
#endif
    );

    static constexpr const char compiler_name[] = ""
#if defined(SYSTEM_COMPILER_MSVC)
        "MSVC"
#endif
#if defined(SYSTEM_COMPILER_MINGW)
    #if defined(SYSTEM_COMPILER_MSVC)
        ","
    #endif
        "MinGW"
#endif
#if defined(SYSTEM_COMPILER_CLANG)
    #if defined(SYSTEM_COMPILER_MSVC) || defined(SYSTEM_COMPILER_MINGW)
        ","
    #endif
        "Clang"
#endif
#if defined(SYSTEM_COMPILER_GCC)
    #if defined(SYSTEM_COMPILER_MSVC) || defined(SYSTEM_COMPILER_MINGW) || defined(SYSTEM_COMPILER_CLANG)
        ","
    #endif
        "GCC"
#endif
#if defined(SYSTEM_COMPILER_APPLE)
    #if defined(SYSTEM_COMPILER_MSVC) || defined(SYSTEM_COMPILER_MINGW) || defined(SYSTEM_COMPILER_CLANG) || defined(SYSTEM_COMPILER_GCC)
        ","
    #endif
        "Apple"
#endif
#if defined(SYSTEM_COMPILER_INTEL)
    #if defined(SYSTEM_COMPILER_MSVC) || defined(SYSTEM_COMPILER_MINGW) || defined(SYSTEM_COMPILER_CLANG) || defined(SYSTEM_COMPILER_GCC) || defined(SYSTEM_COMPILER_APPLE)
        ","
    #endif
        "Intel"
#endif
#if defined(SYSTEM_COMPILER_EMSCRIPTEN)
    #if defined(SYSTEM_COMPILER_MSVC) || defined(SYSTEM_COMPILER_MINGW) || defined(SYSTEM_COMPILER_CLANG) || defined(SYSTEM_COMPILER_GCC) || defined(SYSTEM_COMPILER_APPLE) ||\
        defined(SYSTEM_COMPILER_INTEL)
        ","
    #endif
        "Emscripten"
#endif
#if defined(SYSTEM_COMPILER_ARMCC)
    #if defined(SYSTEM_COMPILER_MSVC) || defined(SYSTEM_COMPILER_MINGW) || defined(SYSTEM_COMPILER_CLANG) || defined(SYSTEM_COMPILER_GCC) || defined(SYSTEM_COMPILER_APPLE) ||\
        defined(SYSTEM_COMPILER_INTEL) || defined(SYSTEM_COMPILER_EMSCRIPTEN)
        ","
    #endif
        "ARMCC"
#endif
#if defined(SYSTEM_COMPILER_BORLAND)
    #if defined(SYSTEM_COMPILER_MSVC) || defined(SYSTEM_COMPILER_MINGW) || defined(SYSTEM_COMPILER_CLANG) || defined(SYSTEM_COMPILER_GCC) || defined(SYSTEM_COMPILER_APPLE) ||\
        defined(SYSTEM_COMPILER_INTEL) || defined(SYSTEM_COMPILER_EMSCRIPTEN) || defined(SYSTEM_COMPILER_ARMCC)
        ","
    #endif
        "Borland"
#endif
#if defined(SYSTEM_COMPILER_IBM)
    #if defined(SYSTEM_COMPILER_MSVC) || defined(SYSTEM_COMPILER_MINGW) || defined(SYSTEM_COMPILER_CLANG) || defined(SYSTEM_COMPILER_GCC) || defined(SYSTEM_COMPILER_APPLE) ||\
        defined(SYSTEM_COMPILER_INTEL) || defined(SYSTEM_COMPILER_EMSCRIPTEN) || defined(SYSTEM_COMPILER_ARMCC) || defined(SYSTEM_COMPILER_BORLAND)
        ","
    #endif
        "IBM"
#endif
#if defined(SYSTEM_COMPILER_SOLARIS)
    #if defined(SYSTEM_COMPILER_MSVC) || defined(SYSTEM_COMPILER_MINGW) || defined(SYSTEM_COMPILER_CLANG) || defined(SYSTEM_COMPILER_GCC) || defined(SYSTEM_COMPILER_APPLE) ||\
        defined(SYSTEM_COMPILER_INTEL) || defined(SYSTEM_COMPILER_EMSCRIPTEN) || defined(SYSTEM_COMPILER_ARMCC) || defined(SYSTEM_COMPILER_BORLAND) || defined(SYSTEM_COMPILER_IBM)
        ","
    #endif
        "Solaris"
#endif
#if defined(SYSTEM_COMPILER_HPUX)
    #if defined(SYSTEM_COMPILER_MSVC) || defined(SYSTEM_COMPILER_MINGW) || defined(SYSTEM_COMPILER_CLANG) || defined(SYSTEM_COMPILER_GCC) || defined(SYSTEM_COMPILER_APPLE) ||\
        defined(SYSTEM_COMPILER_INTEL) || defined(SYSTEM_COMPILER_EMSCRIPTEN) || defined(SYSTEM_COMPILER_ARMCC) || defined(SYSTEM_COMPILER_BORLAND)defined(SYSTEM_COMPILER_IBM) || defined(SYSTEM_COMPILER_SOLARIS)
        ","
    #endif
        "HPUX"
#endif
    ;
}
#endif