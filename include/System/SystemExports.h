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

#ifdef __cplusplus
    #define SYSTEM_EXTERN_NONE
    #define SYSTEM_EXTERN_C   extern "C"
    #define SYSTEM_EXTERN_CXX extern
#else
    #define SYSTEM_EXTERN_NONE
    #define SYSTEM_EXTERN_C   extern
    #define SYSTEM_EXTERN_CXX #error "No C++ export in C"
#endif

#if defined(SYSTEM_OS_WINDOWS)
    #if defined(__clang__)
        #define SYSTEM_CALL_DEFAULT 
        #define SYSTEM_CALL_STDL    __stdcall
        #define SYSTEM_CALL_CDECL   __cdecl
        #define SYSTEM_CALL_FAST    __fastcall
        #define SYSTEM_CALL_THIS    __thiscall

        #define SYSTEM_MODE_DEFAULT
        #define SYSTEM_MODE_EXPORT  __declspec(dllexport)
        #define SYSTEM_MODE_IMPORT  __declspec(dllimport)
        #define SYSTEM_MODE_HIDDEN

        #define SYSTEM_HIDE_CLASS(keyword)                                         SYSTEM_EXTERN_NONE SYSTEM_MODE_HIDDEN keyword
        #define SYSTEM_HIDE_API(return_type, call_convention)                      SYSTEM_EXTERN_NONE SYSTEM_MODE_HIDDEN return_type call_convention
        #define SYSTEM_EXPORT_API(extern_type, return_type, mode, call_convention) extern_type        mode               return_type call_convention
    #else
        #define SYSTEM_CALL_DEFAULT 
        #define SYSTEM_CALL_STDL    __stdcall
        #define SYSTEM_CALL_CDECL   __cdecl
        #define SYSTEM_CALL_FAST    __fastcall
        #define SYSTEM_CALL_THIS    __thiscall

        #define SYSTEM_MODE_DEFAULT
        #define SYSTEM_MODE_EXPORT  __declspec(dllexport)
        #define SYSTEM_MODE_IMPORT  __declspec(dllimport)
        #define SYSTEM_MODE_HIDDEN 

        #define SYSTEM_HIDE_CLASS(keyword)                                         SYSTEM_EXTERN_NONE SYSTEM_MODE_HIDDEN keyword
        #define SYSTEM_HIDE_API(return_type, call_convention)                      SYSTEM_EXTERN_NONE SYSTEM_MODE_HIDDEN return_type call_convention
        #define SYSTEM_EXPORT_API(extern_type, return_type, mode, call_convention) extern_type        mode               return_type call_convention
    #endif
#elif defined(SYSTEM_OS_LINUX) || defined(SYSTEM_OS_APPLE)
    #define SYSTEM_CALL_DEFAULT 
    #define SYSTEM_CALL_STD     __attribute__((stdcall))
    #define SYSTEM_CALL_CDECL   __attribute__((cdecl))
    #define SYSTEM_CALL_FAST    __attribute__((fastcall))
    #define SYSTEM_CALL_THIS    __attribute__((thiscall))

    #define SYSTEM_MODE_DEFAULT
    #define SYSTEM_MODE_EXPORT  __attribute__((visibility("default")))
    #define SYSTEM_MODE_IMPORT  __attribute__((visibility("default")))
    #define SYSTEM_MODE_HIDDEN  __attribute__((visibility("hidden")))

    #define SYSTEM_HIDE_CLASS(keyword)                                         SYSTEM_EXTERN_NONE keyword     SYSTEM_MODE_HIDDEN
    #define SYSTEM_HIDE_API(return_type, call_convention)                      SYSTEM_EXTERN_NONE return_type SYSTEM_MODE_HIDDEN call_convention
    #define SYSTEM_EXPORT_API(extern_type, return_type, mode, call_convention) extern_type        return_type mode               call_convention

    //#define LOCAL_API __attribute__((visibility ("internal")))
#endif

#ifdef WANT_SYSTEM_SHARED_LIBRARY_EXPORTS
void shared_library_load(void* hmodule);
void shared_library_unload(void* hmodule);
#endif
