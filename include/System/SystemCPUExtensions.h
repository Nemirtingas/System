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

#include "SystemDetector.h"

#if defined(SYSTEM_ARCH_X86) || defined(SYSTEM_ARCH_X64)
namespace System {
namespace CpuFeatures {
    union CpuId_t
    {
        struct
        {
            unsigned int eax;
            unsigned int ebx;
            unsigned int ecx;
            unsigned int edx;
        };

        unsigned int registers[4];
    };

    CpuId_t CpuId(int functionIndex);

    struct CpuFeature_t
    {
        unsigned int FunctionIndex;
        unsigned int FeatureRegister : 8;
        unsigned int FeatureFlag : 8;
    };

    static constexpr unsigned int CpuRegisterEAX = 0;
    static constexpr unsigned int CpuRegisterEBX = 1;
    static constexpr unsigned int CpuRegisterECX = 2;
    static constexpr unsigned int CpuRegisterEDX = 3;

    static constexpr auto FPU                 = CpuFeature_t{ 1, CpuRegisterEDX, 0  };
    static constexpr auto VME                 = CpuFeature_t{ 1, CpuRegisterEDX, 1  };
    static constexpr auto DE                  = CpuFeature_t{ 1, CpuRegisterEDX, 2  };
    static constexpr auto PSE                 = CpuFeature_t{ 1, CpuRegisterEDX, 3  };
    static constexpr auto TSC                 = CpuFeature_t{ 1, CpuRegisterEDX, 4  };
    static constexpr auto MSR                 = CpuFeature_t{ 1, CpuRegisterEDX, 5  };
    static constexpr auto PAE                 = CpuFeature_t{ 1, CpuRegisterEDX, 6  };
    static constexpr auto MCE                 = CpuFeature_t{ 1, CpuRegisterEDX, 7  };
    static constexpr auto CX8                 = CpuFeature_t{ 1, CpuRegisterEDX, 8  };
    static constexpr auto APIC                = CpuFeature_t{ 1, CpuRegisterEDX, 9  };
    static constexpr auto INDEX_1_EDX_10      = CpuFeature_t{ 1, CpuRegisterEDX, 10 };
    static constexpr auto SEP                 = CpuFeature_t{ 1, CpuRegisterEDX, 11 };
    static constexpr auto MTRR                = CpuFeature_t{ 1, CpuRegisterEDX, 12 };
    static constexpr auto PGE                 = CpuFeature_t{ 1, CpuRegisterEDX, 13 };
    static constexpr auto MCA                 = CpuFeature_t{ 1, CpuRegisterEDX, 14 };
    static constexpr auto CMOV                = CpuFeature_t{ 1, CpuRegisterEDX, 15 };
    static constexpr auto PAT                 = CpuFeature_t{ 1, CpuRegisterEDX, 16 };
    static constexpr auto PSE36               = CpuFeature_t{ 1, CpuRegisterEDX, 17 };
    static constexpr auto PSN                 = CpuFeature_t{ 1, CpuRegisterEDX, 18 };
    static constexpr auto CLFSH               = CpuFeature_t{ 1, CpuRegisterEDX, 19 };
    static constexpr auto INDEX_1_EDX_20      = CpuFeature_t{ 1, CpuRegisterEDX, 20 };
    static constexpr auto DS                  = CpuFeature_t{ 1, CpuRegisterEDX, 21 };
    static constexpr auto ACPI                = CpuFeature_t{ 1, CpuRegisterEDX, 22 };
    static constexpr auto MMX                 = CpuFeature_t{ 1, CpuRegisterEDX, 23 };
    static constexpr auto FXSR                = CpuFeature_t{ 1, CpuRegisterEDX, 24 };
    static constexpr auto SSE                 = CpuFeature_t{ 1, CpuRegisterEDX, 25 };
    static constexpr auto SSE2                = CpuFeature_t{ 1, CpuRegisterEDX, 26 };
    static constexpr auto SS                  = CpuFeature_t{ 1, CpuRegisterEDX, 27 };
    static constexpr auto HTT                 = CpuFeature_t{ 1, CpuRegisterEDX, 28 };
    static constexpr auto TM                  = CpuFeature_t{ 1, CpuRegisterEDX, 29 };
    static constexpr auto INDEX_1_EDX_30      = CpuFeature_t{ 1, CpuRegisterEDX, 30 };
    static constexpr auto PBE                 = CpuFeature_t{ 1, CpuRegisterEDX, 31 };

    static constexpr auto SSE3                = CpuFeature_t{ 1, CpuRegisterECX, 0  };
    static constexpr auto PCLMULQDQ           = CpuFeature_t{ 1, CpuRegisterECX, 1  };
    static constexpr auto DTES64              = CpuFeature_t{ 1, CpuRegisterECX, 2  };
    static constexpr auto MONITOR             = CpuFeature_t{ 1, CpuRegisterECX, 3  };
    static constexpr auto DS_CPL              = CpuFeature_t{ 1, CpuRegisterECX, 4  };
    static constexpr auto VMX                 = CpuFeature_t{ 1, CpuRegisterECX, 5  };
    static constexpr auto SMX                 = CpuFeature_t{ 1, CpuRegisterECX, 6  };
    static constexpr auto EIST                = CpuFeature_t{ 1, CpuRegisterECX, 7  };
    static constexpr auto TM2                 = CpuFeature_t{ 1, CpuRegisterECX, 8  };
    static constexpr auto SSSE3               = CpuFeature_t{ 1, CpuRegisterECX, 9  };
    static constexpr auto CNXT_ID             = CpuFeature_t{ 1, CpuRegisterECX, 10 };
    static constexpr auto SDBG                = CpuFeature_t{ 1, CpuRegisterECX, 11 };
    static constexpr auto FMA                 = CpuFeature_t{ 1, CpuRegisterECX, 12 };
    static constexpr auto CMPXCHG16B          = CpuFeature_t{ 1, CpuRegisterECX, 13 };
    static constexpr auto XTPRUPDCTRL         = CpuFeature_t{ 1, CpuRegisterECX, 14 };
    static constexpr auto PDCM                = CpuFeature_t{ 1, CpuRegisterECX, 15 };
    static constexpr auto PCID                = CpuFeature_t{ 1, CpuRegisterECX, 17 };
    static constexpr auto DCA                 = CpuFeature_t{ 1, CpuRegisterECX, 18 };
    static constexpr auto SSE4_1              = CpuFeature_t{ 1, CpuRegisterECX, 19 };
    static constexpr auto SSE4_2              = CpuFeature_t{ 1, CpuRegisterECX, 20 };
    static constexpr auto X2APIC              = CpuFeature_t{ 1, CpuRegisterECX, 21 };
    static constexpr auto MOVBE               = CpuFeature_t{ 1, CpuRegisterECX, 22 };
    static constexpr auto POPCNT              = CpuFeature_t{ 1, CpuRegisterECX, 23 };
    static constexpr auto TSC_DEADLINE        = CpuFeature_t{ 1, CpuRegisterECX, 24 };
    static constexpr auto AES                 = CpuFeature_t{ 1, CpuRegisterECX, 25 };
    static constexpr auto XSAVE               = CpuFeature_t{ 1, CpuRegisterECX, 26 };
    static constexpr auto OSXSAVE             = CpuFeature_t{ 1, CpuRegisterECX, 27 };
    static constexpr auto AVX                 = CpuFeature_t{ 1, CpuRegisterECX, 28 };
    static constexpr auto F16C                = CpuFeature_t{ 1, CpuRegisterECX, 29 };
    static constexpr auto RDRAND              = CpuFeature_t{ 1, CpuRegisterECX, 30 };
    static constexpr auto INDEX_1_ECX_31      = CpuFeature_t{ 1, CpuRegisterECX, 31 };

    static constexpr auto FSGSBASE            = CpuFeature_t{ 7, CpuRegisterEBX, 0  };
    static constexpr auto TSC_ADJUST          = CpuFeature_t{ 7, CpuRegisterEBX, 1  };
    static constexpr auto SGX                 = CpuFeature_t{ 7, CpuRegisterEBX, 2  };
    static constexpr auto BMI1                = CpuFeature_t{ 7, CpuRegisterEBX, 3  };
    static constexpr auto HLE                 = CpuFeature_t{ 7, CpuRegisterEBX, 4  };
    static constexpr auto AVX2                = CpuFeature_t{ 7, CpuRegisterEBX, 5  };
    static constexpr auto INDEX_7_EBX_6       = CpuFeature_t{ 7, CpuRegisterEBX, 6  };
    static constexpr auto SMEP                = CpuFeature_t{ 7, CpuRegisterEBX, 7  };
    static constexpr auto BMI2                = CpuFeature_t{ 7, CpuRegisterEBX, 8  };
    static constexpr auto ERMS                = CpuFeature_t{ 7, CpuRegisterEBX, 9  };
    static constexpr auto INVPCID             = CpuFeature_t{ 7, CpuRegisterEBX, 10 };
    static constexpr auto RTM                 = CpuFeature_t{ 7, CpuRegisterEBX, 11 };
    static constexpr auto RDT_M               = CpuFeature_t{ 7, CpuRegisterEBX, 12 };
    static constexpr auto DEPR_FPU_CS_DS      = CpuFeature_t{ 7, CpuRegisterEBX, 13 };
    static constexpr auto MPX                 = CpuFeature_t{ 7, CpuRegisterEBX, 14 };
    static constexpr auto RDT_A               = CpuFeature_t{ 7, CpuRegisterEBX, 15 };
    static constexpr auto AVX512F             = CpuFeature_t{ 7, CpuRegisterEBX, 16 };
    static constexpr auto AVX512DQ            = CpuFeature_t{ 7, CpuRegisterEBX, 17 };
    static constexpr auto RDSEED              = CpuFeature_t{ 7, CpuRegisterEBX, 18 };
    static constexpr auto ADX                 = CpuFeature_t{ 7, CpuRegisterEBX, 19 };
    static constexpr auto SMAP                = CpuFeature_t{ 7, CpuRegisterEBX, 20 };
    static constexpr auto AVX512_IFMA         = CpuFeature_t{ 7, CpuRegisterEBX, 21 };
    static constexpr auto INDEX_7_EBX_22      = CpuFeature_t{ 7, CpuRegisterEBX, 22 };
    static constexpr auto CLFLUSHOPT          = CpuFeature_t{ 7, CpuRegisterEBX, 23 };
    static constexpr auto CLWB                = CpuFeature_t{ 7, CpuRegisterEBX, 24 };
    static constexpr auto TRACE               = CpuFeature_t{ 7, CpuRegisterEBX, 25 };
    static constexpr auto AVX512PF            = CpuFeature_t{ 7, CpuRegisterEBX, 26 };
    static constexpr auto AVX512ER            = CpuFeature_t{ 7, CpuRegisterEBX, 27 };
    static constexpr auto AVX512CD            = CpuFeature_t{ 7, CpuRegisterEBX, 28 };
    static constexpr auto SHA                 = CpuFeature_t{ 7, CpuRegisterEBX, 29 };
    static constexpr auto AVX512BW            = CpuFeature_t{ 7, CpuRegisterEBX, 30 };
    static constexpr auto AVX512VL            = CpuFeature_t{ 7, CpuRegisterEBX, 31 };

    static constexpr auto PREFETCHWT1         = CpuFeature_t{ 7, CpuRegisterECX, 0  };
    static constexpr auto AVX512_VBMI         = CpuFeature_t{ 7, CpuRegisterECX, 1  };
    static constexpr auto UMIP                = CpuFeature_t{ 7, CpuRegisterECX, 2  };
    static constexpr auto PKU                 = CpuFeature_t{ 7, CpuRegisterECX, 3  };
    static constexpr auto OSPKE               = CpuFeature_t{ 7, CpuRegisterECX, 4  };
    static constexpr auto WAITPKG             = CpuFeature_t{ 7, CpuRegisterECX, 5  };
    static constexpr auto AVX512_VBMI2        = CpuFeature_t{ 7, CpuRegisterECX, 6  };
    static constexpr auto SHSTK               = CpuFeature_t{ 7, CpuRegisterECX, 7  };
    static constexpr auto GFNI                = CpuFeature_t{ 7, CpuRegisterECX, 8  };
    static constexpr auto VAES                = CpuFeature_t{ 7, CpuRegisterECX, 9  };
    static constexpr auto VPCLMULQDQ          = CpuFeature_t{ 7, CpuRegisterECX, 10 };
    static constexpr auto AVX512_VNNI         = CpuFeature_t{ 7, CpuRegisterECX, 11 };
    static constexpr auto AVX512_BITALG       = CpuFeature_t{ 7, CpuRegisterECX, 12 };
    static constexpr auto INDEX_7_ECX_13      = CpuFeature_t{ 7, CpuRegisterECX, 13 };
    static constexpr auto AVX512_VPOPCNTDQ    = CpuFeature_t{ 7, CpuRegisterECX, 14 };
    static constexpr auto INDEX_7_ECX_15      = CpuFeature_t{ 7, CpuRegisterECX, 15 };
    static constexpr auto INDEX_7_ECX_16      = CpuFeature_t{ 7, CpuRegisterECX, 16 };
    static constexpr auto RDPID               = CpuFeature_t{ 7, CpuRegisterECX, 22 };
    static constexpr auto KL                  = CpuFeature_t{ 7, CpuRegisterECX, 23 };
    static constexpr auto INDEX_7_ECX_24      = CpuFeature_t{ 7, CpuRegisterECX, 24 };
    static constexpr auto CLDEMOTE            = CpuFeature_t{ 7, CpuRegisterECX, 25 };
    static constexpr auto INDEX_7_ECX_26      = CpuFeature_t{ 7, CpuRegisterECX, 26 };
    static constexpr auto MOVDIRI             = CpuFeature_t{ 7, CpuRegisterECX, 27 };
    static constexpr auto MOVDIR64B           = CpuFeature_t{ 7, CpuRegisterECX, 28 };
    static constexpr auto ENQCMD              = CpuFeature_t{ 7, CpuRegisterECX, 29 };
    static constexpr auto SGX_LC              = CpuFeature_t{ 7, CpuRegisterECX, 30 };
    static constexpr auto PKS                 = CpuFeature_t{ 7, CpuRegisterECX, 31 };

    static constexpr auto INDEX_7_EDX_0       = CpuFeature_t{ 7, CpuRegisterEDX, 0  };
    static constexpr auto INDEX_7_EDX_1       = CpuFeature_t{ 7, CpuRegisterEDX, 1  };
    static constexpr auto AVX512_4VNNIW       = CpuFeature_t{ 7, CpuRegisterEDX, 2  };
    static constexpr auto AVX512_4FMAPS       = CpuFeature_t{ 7, CpuRegisterEDX, 3  };
    static constexpr auto FSRM                = CpuFeature_t{ 7, CpuRegisterEDX, 4  };
    static constexpr auto UINTR               = CpuFeature_t{ 7, CpuRegisterEDX, 5  };
    static constexpr auto INDEX_7_EDX_6       = CpuFeature_t{ 7, CpuRegisterEDX, 6  };
    static constexpr auto INDEX_7_EDX_7       = CpuFeature_t{ 7, CpuRegisterEDX, 7  };
    static constexpr auto AVX512_VP2INTERSECT = CpuFeature_t{ 7, CpuRegisterEDX, 8  };
    static constexpr auto INDEX_7_EDX_9       = CpuFeature_t{ 7, CpuRegisterEDX, 9  };
    static constexpr auto MD_CLEAR            = CpuFeature_t{ 7, CpuRegisterEDX, 10 };
    static constexpr auto RTM_ALWAYS_ABORT    = CpuFeature_t{ 7, CpuRegisterEDX, 11 };
    static constexpr auto INDEX_7_EDX_12      = CpuFeature_t{ 7, CpuRegisterEDX, 12 };
    static constexpr auto INDEX_7_EDX_13      = CpuFeature_t{ 7, CpuRegisterEDX, 13 };
    static constexpr auto SERIALIZE           = CpuFeature_t{ 7, CpuRegisterEDX, 14 };
    static constexpr auto HYBRID              = CpuFeature_t{ 7, CpuRegisterEDX, 15 };
    static constexpr auto TSXLDTRK            = CpuFeature_t{ 7, CpuRegisterEDX, 16 };
    static constexpr auto INDEX_7_EDX_17      = CpuFeature_t{ 7, CpuRegisterEDX, 17 };
    static constexpr auto PCONFIG             = CpuFeature_t{ 7, CpuRegisterEDX, 18 };
    static constexpr auto INDEX_7_EDX_19      = CpuFeature_t{ 7, CpuRegisterEDX, 19 };
    static constexpr auto IBT                 = CpuFeature_t{ 7, CpuRegisterEDX, 20 };
    static constexpr auto INDEX_7_EDX_21      = CpuFeature_t{ 7, CpuRegisterEDX, 21 };
    static constexpr auto AMX_BF16            = CpuFeature_t{ 7, CpuRegisterEDX, 22 };
    static constexpr auto AVX512_FP16         = CpuFeature_t{ 7, CpuRegisterEDX, 23 };
    static constexpr auto AMX_TILE            = CpuFeature_t{ 7, CpuRegisterEDX, 24 };
    static constexpr auto AMX_INT8            = CpuFeature_t{ 7, CpuRegisterEDX, 25 };
    static constexpr auto IBRS_IBPB           = CpuFeature_t{ 7, CpuRegisterEDX, 26 };
    static constexpr auto STIBP               = CpuFeature_t{ 7, CpuRegisterEDX, 27 };
    static constexpr auto L1D_FLUSH           = CpuFeature_t{ 7, CpuRegisterEDX, 28 };
    static constexpr auto ARCH_CAPABILITIES   = CpuFeature_t{ 7, CpuRegisterEDX, 29 };
    static constexpr auto CORE_CAPABILITIES   = CpuFeature_t{ 7, CpuRegisterEDX, 30 };
    static constexpr auto SSBD                = CpuFeature_t{ 7, CpuRegisterEDX, 31 };

    static constexpr auto AVX_VNNI            = CpuFeature_t{ 7, CpuRegisterEDX, 4  };
    static constexpr auto AVX512_BF16         = CpuFeature_t{ 7, CpuRegisterEDX, 5  };
    static constexpr auto FZLRM               = CpuFeature_t{ 7, CpuRegisterEDX, 10 };
    static constexpr auto FSRS                = CpuFeature_t{ 7, CpuRegisterEDX, 11 };
    static constexpr auto FSRCS               = CpuFeature_t{ 7, CpuRegisterEDX, 12 };
    static constexpr auto HRESET              = CpuFeature_t{ 7, CpuRegisterEDX, 22 };
    static constexpr auto LAM                 = CpuFeature_t{ 7, CpuRegisterEDX, 26 };

    inline constexpr bool HasFeature(CpuId_t cpuId, CpuFeatures::CpuFeature_t feature)
    {
        return (cpuId.registers[feature.FeatureRegister] & (1 << feature.FeatureFlag)) != 0;
    }
}// namespace CpuFeatures
}// namespace System

#elif defined(SYSTEM_ARCH_ARM) || defined(SYSTEM_ARCH_ARM64)

// ARM's cpuid instructions are reserved to kernel mode

#endif

