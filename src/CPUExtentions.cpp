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

#include <System/SystemCPUExtensions.h>

#if defined(SYSTEM_ARCH_X86) || defined(SYSTEM_ARCH_X64)
#if defined(__clang__) || defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)
namespace System {
namespace CpuFeatures {
    CpuId_t CpuId(int functionIndex)
    {
        CpuId_t cpuId;
        __asm__ __volatile__(
            "cpuid"
            : "=a"(cpuId.Registers.eax), "=b"(cpuId.Registers.ebx), "=c"(cpuId.Registers.ecx), "=d"(cpuId.Registers.edx)
            : "a"(functionIndex)
        );

        return cpuId;
    }
}// namespace CpuFeatures
}// namespace System
#else
#include <intrin.h>

namespace System {
namespace CpuFeatures {
    CpuId_t CpuId(int functionIndex)
    {
        CpuId_t cpuId;
        __cpuidex((int*)cpuId.RegisterArray, functionIndex, 0);

        return cpuId;
    }
}// namespace CpuFeatures
}// namespace System
#endif
#elif defined(SYSTEM_ARCH_ARM) || defined(SYSTEM_ARCH_ARM64)
#if defined(__clang__) || defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)
//namespace System {
//namespace CpuFeatures {
//    CpuId_t CpuId(int functionIndex)
//    {
//        CpuId_t cpuId;
//        __asm__ __volatile__(
//            "cpuid"
//            : "=a"(cpuId.eax), "=b"(cpuId.ebx), "=c"(cpuId.ecx), "=d"(cpuId.edx)
//            : "a"(functionIndex)
//        );
//
//        return cpuId;
//    }
//}// namespace CpuFeatures
//}// namespace System
#else
//#include <intrin.h>
//
//namespace System {
//namespace CpuFeatures {
//    CpuId_t CpuId(int functionIndex)
//    {
//        CpuId_t cpuId;
//        __cpuidex((int*)cpuId.registers, functionIndex, 0);
//
//        return cpuId;
//    }
//}// namespace CpuFeatures
//}// namespace System
#endif
#endif
