#include <System/System.h>
#include <System/Library.h>
#include <System/Filesystem.h>
#include <System/SystemDetector.h>
#include <System/String.hpp>
#include <System/Encoding.hpp>
#include <System/StringSwitch.hpp>
#include <System/Guid.hpp>
#include <System/SystemMacro.h>
#include <System/SystemCompiler.h>
#include <System/SystemCPUExtensions.h>
#include <System/LoopBreak.hpp>
#include <System/FunctionName.hpp>
#include <System/DotNet.hpp>

#include <vector>
#include <variant>
#include <iostream>
#include <memory>
#include <fstream>
#include <map>

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#define TEST_MACRO_1(a1) a1
#define TEST_MACRO_2(a1, a2) TEST_MACRO_1(a1) + a2
#define TEST_MACRO_3(a1, a2, a3) TEST_MACRO_2(a1, a2) + a3
#define TEST_MACRO_4(a1, a2, a3, a4) TEST_MACRO_3(a1, a2, a3) + a4
#define TEST_MACRO_5(a1, a2, a3, a4, a5) TEST_MACRO_4(a1, a2, a3, a4) + a5

#ifdef CreateDirectory
#undef CreateDirectory
#endif

/*
"get_hostfxr_path" // buffer, 1024, nullptr

"hostfxr_initialize_for_runtime_config" // 
typedef int32_t(HOSTFXR_CALLTYPE* hostfxr_initialize_for_runtime_config_fn)(
    const char_t* runtime_config_path,
    const struct hostfxr_initialize_parameters* parameters,
    out hostfxr_handle* host_context_handle);

"hostfxr_get_runtime_delegate" // 
typedef int32_t(HOSTFXR_CALLTYPE* hostfxr_get_runtime_delegate_fn)(
    const hostfxr_handle host_context_handle,
    enum hostfxr_delegate_type type,
    out void** delegate);

"hostfxr_close" //
typedef int32_t(HOSTFXR_CALLTYPE* hostfxr_close_fn)(const hostfxr_handle host_context_handle);

hostfxr_handle ctx = nullptr;

u32 result = hostfxr_initialize_for_runtime_config(path.c_str(), nullptr, &ctx);
ON_SCOPE_EXIT{
    hostfxr_close(ctx);
};

if (result > 2 || ctx == nullptr) {
    throw std::runtime_error(hex::format("Failed to initialize command line {:X}", result));
}

result = hostfxr_get_runtime_delegate(
    ctx,
    hostfxr_delegate_type::hdt_load_assembly_and_get_function_pointer,
    (void**)&loadAssemblyFunction
);
*/

int main(int argc, char *argv[])
{
    return Catch::Session().run(argc, argv);
}

auto globalNamespaceLambda = []() {
    std::cout << SYSTEM_DETAILS_FUNCTION_NAME << " | " << SYSTEM_FUNCTION_NAME << std::endl;
};

template<typename T>
static inline std::weak_ptr<T> make_weak(std::shared_ptr<T> v)
{
    return v;
}

class ClangOperatorWithoutSpace
{
    struct InternalStruct_t
    {

    };

public:
    void _DoSomething(std::shared_ptr<InternalStruct_t> a1, uint64_t const& a2, std::string const& a3)
    {
        [this, aa1 = make_weak(a1)](const std::variant<std::vector<uint8_t>, std::string>& message) {
            CHECK(SYSTEM_FUNCTION_NAME == std::string{ "ClangOperatorWithoutSpace::_DoSomething" });
            std::cout << SYSTEM_DETAILS_FUNCTION_NAME << " | " << SYSTEM_FUNCTION_NAME << std::endl;
        }("");
    }
};

struct FunctionNameStructTest
{
    int Name1(int, int)
    {
        CHECK(SYSTEM_FUNCTION_NAME == std::string{ "FunctionNameStructTest::Name1" });
        std::cout << SYSTEM_DETAILS_FUNCTION_NAME << " | " << SYSTEM_FUNCTION_NAME << std::endl;
        return 0;
    }

    template<typename T, typename U, typename V>
    V Name2(T, U)
    {
        CHECK(SYSTEM_FUNCTION_NAME == std::string_view{ "FunctionNameStructTest::Name2" });
        std::cout << SYSTEM_DETAILS_FUNCTION_NAME << " | " << SYSTEM_FUNCTION_NAME << std::endl;
        return {};
    }

    void Lambda1()
    {
        [this]() {
            CHECK(SYSTEM_FUNCTION_NAME == std::string_view{ "FunctionNameStructTest::Lambda1" });
            std::cout << SYSTEM_DETAILS_FUNCTION_NAME << " | " << SYSTEM_FUNCTION_NAME << std::endl;
        }();

        []() {
            []() {
                CHECK(SYSTEM_FUNCTION_NAME == std::string_view{ "FunctionNameStructTest::Lambda1" });
                std::cout << SYSTEM_DETAILS_FUNCTION_NAME << " | " << SYSTEM_FUNCTION_NAME << std::endl;
            }();
        }();
    }
};

TEST_CASE("Function name extractor, [function_name]")
{
    globalNamespaceLambda();
    FunctionNameStructTest test1;
    test1.Name1(1, 1);
    test1.Name2<int, float, char>(5, 3);
    test1.Lambda1();

    ClangOperatorWithoutSpace clangTest;
    uint64_t someInteger;
    clangTest._DoSomething(nullptr, someInteger, "");
}

TEST_CASE("Filesystem", "[filesystem]")
{
    auto executableDir = System::Filesystem::Dirname(System::GetExecutablePath());

    CHECK(System::Filesystem::Exists(executableDir));
    CHECK(System::Filesystem::IsDir(executableDir));
    CHECK(System::Filesystem::IsFile(System::GetExecutablePath()));

    CHECK_FALSE(System::Filesystem::Exists(System::Filesystem::Join(executableDir, "dir_test", "subdir_test")));
    CHECK_FALSE(System::Filesystem::CreateDirectory(System::Filesystem::Join(executableDir, "dir_test", "subdir_test"), false));
    CHECK(System::Filesystem::CreateDirectory(System::Filesystem::Join(executableDir, "dir_test", "subdir_test"), true));
    CHECK(System::Filesystem::Exists(System::Filesystem::Join(executableDir, "dir_test")));
    CHECK(System::Filesystem::Exists(System::Filesystem::Join(executableDir, "dir_test", "subdir_test")));

    CHECK_FALSE(System::Filesystem::Exists(System::Filesystem::Join(executableDir, "dir_test", "subdir_test", "file_test")));
    std::ofstream(System::Filesystem::Join(executableDir, "dir_test", "subdir_test", "file_test"), std::ios::binary | std::ios::out | std::ios::trunc);

    CHECK(System::Filesystem::Exists(System::Filesystem::Join(executableDir, "dir_test", "subdir_test", "file_test")));
    CHECK(System::Filesystem::IsFile(System::Filesystem::Join(executableDir, "dir_test", "subdir_test", "file_test")));
}

TEST_CASE("Nested loop break", "[nested loop break]")
{
    int valueI = 0, valueJ = 0, valueK = 0;
    for (int i = 0; i < 0xff; ++i) SYSTEM_LOOP_NAME(VALUE_I)
    {
        ++valueI;
        for (int j = 0; j < 5; ++j) SYSTEM_LOOP_NAME(VALUE_J)
        {
            for (int k = 0; k < 16; ++k)
            {
                if (k > 0x8)
                    SYSTEM_LOOP_CONTINUE(VALUE_I);

                if (i > 3)
                    SYSTEM_LOOP_BREAK(VALUE_I);

                ++valueK;
            }

            ++valueJ;
        }
    }

    CHECK(valueK == 36);
    CHECK(valueJ == 0);
    CHECK(valueI == 5);
}

TEST_CASE("CpuId", "[cpuid]")
{
#if defined(SYSTEM_ARCH_X64) || defined(SYSTEM_ARCH_X86)
    char cpuName[13]{};

    System::CpuFeatures::CpuId_t cpuId0 = System::CpuFeatures::CpuId(0);
    *(int32_t*)&(cpuName[0]) = cpuId0.Registers.ebx;
    *(int32_t*)&(cpuName[4]) = cpuId0.Registers.edx;
    *(int32_t*)&(cpuName[8]) = cpuId0.Registers.ecx;

    std::cout << "CPU ID             : " << cpuName << std::endl;
#define CHECK_CPU_FEATURE(CPUID, FEATURE) printf("%-19s: %s\n", #FEATURE, System::CpuFeatures::HasFeature(CPUID, System::CpuFeatures:: FEATURE) ? "YES":"NO")
    if (cpuId0.Registers.eax >= 1)
    {
        System::CpuFeatures::CpuId_t cpuId1 = System::CpuFeatures::CpuId(1);
        CHECK_CPU_FEATURE(cpuId1, FPU);
        CHECK_CPU_FEATURE(cpuId1, VME);
        CHECK_CPU_FEATURE(cpuId1, DE);
        CHECK_CPU_FEATURE(cpuId1, PSE);
        CHECK_CPU_FEATURE(cpuId1, TSC);
        CHECK_CPU_FEATURE(cpuId1, MSR);
        CHECK_CPU_FEATURE(cpuId1, PAE);
        CHECK_CPU_FEATURE(cpuId1, MCE);
        CHECK_CPU_FEATURE(cpuId1, CX8);
        CHECK_CPU_FEATURE(cpuId1, APIC);
        //CHECK_CPU_FEATURE(cpuId1, INDEX_1_EDX_10);
        CHECK_CPU_FEATURE(cpuId1, SEP);
        CHECK_CPU_FEATURE(cpuId1, MTRR);
        CHECK_CPU_FEATURE(cpuId1, PGE);
        CHECK_CPU_FEATURE(cpuId1, MCA);
        CHECK_CPU_FEATURE(cpuId1, CMOV);
        CHECK_CPU_FEATURE(cpuId1, PAT);
        CHECK_CPU_FEATURE(cpuId1, PSE36);
        CHECK_CPU_FEATURE(cpuId1, PSN);
        CHECK_CPU_FEATURE(cpuId1, CLFSH);
        //CHECK_CPU_FEATURE(cpuId1, INDEX_1_EDX_20);
        CHECK_CPU_FEATURE(cpuId1, DS);
        CHECK_CPU_FEATURE(cpuId1, ACPI);
        CHECK_CPU_FEATURE(cpuId1, MMX);
        CHECK_CPU_FEATURE(cpuId1, FXSR);
        CHECK_CPU_FEATURE(cpuId1, SSE);
        CHECK_CPU_FEATURE(cpuId1, SSE2);
        CHECK_CPU_FEATURE(cpuId1, SS);
        CHECK_CPU_FEATURE(cpuId1, HTT);
        CHECK_CPU_FEATURE(cpuId1, TM);
        //CHECK_CPU_FEATURE(cpuId1, INDEX_1_EDX_30);
        CHECK_CPU_FEATURE(cpuId1, PBE);
        CHECK_CPU_FEATURE(cpuId1, SSE3);
        CHECK_CPU_FEATURE(cpuId1, PCLMULQDQ);
        CHECK_CPU_FEATURE(cpuId1, DTES64);
        CHECK_CPU_FEATURE(cpuId1, MONITOR);
        CHECK_CPU_FEATURE(cpuId1, DS_CPL);
        CHECK_CPU_FEATURE(cpuId1, VMX);
        CHECK_CPU_FEATURE(cpuId1, SMX);
        CHECK_CPU_FEATURE(cpuId1, EIST);
        CHECK_CPU_FEATURE(cpuId1, TM2);
        CHECK_CPU_FEATURE(cpuId1, SSSE3);
        CHECK_CPU_FEATURE(cpuId1, CNXT_ID);
        CHECK_CPU_FEATURE(cpuId1, SDBG);
        CHECK_CPU_FEATURE(cpuId1, FMA);
        CHECK_CPU_FEATURE(cpuId1, CMPXCHG16B);
        CHECK_CPU_FEATURE(cpuId1, XTPRUPDCTRL);
        CHECK_CPU_FEATURE(cpuId1, PDCM);
        CHECK_CPU_FEATURE(cpuId1, PCID);
        CHECK_CPU_FEATURE(cpuId1, DCA);
        CHECK_CPU_FEATURE(cpuId1, SSE4_1);
        CHECK_CPU_FEATURE(cpuId1, SSE4_2);
        CHECK_CPU_FEATURE(cpuId1, X2APIC);
        CHECK_CPU_FEATURE(cpuId1, MOVBE);
        CHECK_CPU_FEATURE(cpuId1, POPCNT);
        CHECK_CPU_FEATURE(cpuId1, TSC_DEADLINE);
        CHECK_CPU_FEATURE(cpuId1, AES);
        CHECK_CPU_FEATURE(cpuId1, XSAVE);
        CHECK_CPU_FEATURE(cpuId1, OSXSAVE);
        CHECK_CPU_FEATURE(cpuId1, AVX);
        CHECK_CPU_FEATURE(cpuId1, F16C);
        CHECK_CPU_FEATURE(cpuId1, RDRAND);
        //CHECK_CPU_FEATURE(cpuId1, INDEX_1_ECX_31);
    }

    if (cpuId0.Registers.eax >= 7)
    {
        System::CpuFeatures::CpuId_t cpuId7 = System::CpuFeatures::CpuId(7);

        CHECK_CPU_FEATURE(cpuId7, FSGSBASE);
        CHECK_CPU_FEATURE(cpuId7, TSC_ADJUST);
        CHECK_CPU_FEATURE(cpuId7, SGX);
        CHECK_CPU_FEATURE(cpuId7, BMI1);
        CHECK_CPU_FEATURE(cpuId7, HLE);
        CHECK_CPU_FEATURE(cpuId7, AVX2);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_EBX_6);
        CHECK_CPU_FEATURE(cpuId7, SMEP);
        CHECK_CPU_FEATURE(cpuId7, BMI2);
        CHECK_CPU_FEATURE(cpuId7, ERMS);
        CHECK_CPU_FEATURE(cpuId7, INVPCID);
        CHECK_CPU_FEATURE(cpuId7, RTM);
        CHECK_CPU_FEATURE(cpuId7, RDT_M);
        CHECK_CPU_FEATURE(cpuId7, DEPR_FPU_CS_DS);
        CHECK_CPU_FEATURE(cpuId7, MPX);
        CHECK_CPU_FEATURE(cpuId7, RDT_A);
        CHECK_CPU_FEATURE(cpuId7, AVX512F);
        CHECK_CPU_FEATURE(cpuId7, AVX512DQ);
        CHECK_CPU_FEATURE(cpuId7, RDSEED);
        CHECK_CPU_FEATURE(cpuId7, ADX);
        CHECK_CPU_FEATURE(cpuId7, SMAP);
        CHECK_CPU_FEATURE(cpuId7, AVX512_IFMA);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_EBX_22);
        CHECK_CPU_FEATURE(cpuId7, CLFLUSHOPT);
        CHECK_CPU_FEATURE(cpuId7, CLWB);
        CHECK_CPU_FEATURE(cpuId7, TRACE);
        CHECK_CPU_FEATURE(cpuId7, AVX512PF);
        CHECK_CPU_FEATURE(cpuId7, AVX512ER);
        CHECK_CPU_FEATURE(cpuId7, AVX512CD);
        CHECK_CPU_FEATURE(cpuId7, SHA);
        CHECK_CPU_FEATURE(cpuId7, AVX512BW);
        CHECK_CPU_FEATURE(cpuId7, AVX512VL);
        CHECK_CPU_FEATURE(cpuId7, PREFETCHWT1);
        CHECK_CPU_FEATURE(cpuId7, AVX512_VBMI);
        CHECK_CPU_FEATURE(cpuId7, UMIP);
        CHECK_CPU_FEATURE(cpuId7, PKU);
        CHECK_CPU_FEATURE(cpuId7, OSPKE);
        CHECK_CPU_FEATURE(cpuId7, WAITPKG);
        CHECK_CPU_FEATURE(cpuId7, AVX512_VBMI2);
        CHECK_CPU_FEATURE(cpuId7, SHSTK);
        CHECK_CPU_FEATURE(cpuId7, GFNI);
        CHECK_CPU_FEATURE(cpuId7, VAES);
        CHECK_CPU_FEATURE(cpuId7, VPCLMULQDQ);
        CHECK_CPU_FEATURE(cpuId7, AVX512_VNNI);
        CHECK_CPU_FEATURE(cpuId7, AVX512_BITALG);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_ECX_13);
        CHECK_CPU_FEATURE(cpuId7, AVX512_VPOPCNTDQ);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_ECX_15);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_ECX_16);
        CHECK_CPU_FEATURE(cpuId7, RDPID);
        CHECK_CPU_FEATURE(cpuId7, KL);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_ECX_24);
        CHECK_CPU_FEATURE(cpuId7, CLDEMOTE);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_ECX_26);
        CHECK_CPU_FEATURE(cpuId7, MOVDIRI);
        CHECK_CPU_FEATURE(cpuId7, MOVDIR64B);
        CHECK_CPU_FEATURE(cpuId7, ENQCMD);
        CHECK_CPU_FEATURE(cpuId7, SGX_LC);
        CHECK_CPU_FEATURE(cpuId7, PKS);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_EDX_0);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_EDX_1);
        CHECK_CPU_FEATURE(cpuId7, AVX512_4VNNIW);
        CHECK_CPU_FEATURE(cpuId7, AVX512_4FMAPS);
        CHECK_CPU_FEATURE(cpuId7, FSRM);
        CHECK_CPU_FEATURE(cpuId7, UINTR);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_EDX_6);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_EDX_7);
        CHECK_CPU_FEATURE(cpuId7, AVX512_VP2INTERSECT);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_EDX_9);
        CHECK_CPU_FEATURE(cpuId7, MD_CLEAR);
        CHECK_CPU_FEATURE(cpuId7, RTM_ALWAYS_ABORT);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_EDX_12);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_EDX_13);
        CHECK_CPU_FEATURE(cpuId7, SERIALIZE);
        CHECK_CPU_FEATURE(cpuId7, HYBRID);
        CHECK_CPU_FEATURE(cpuId7, TSXLDTRK);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_EDX_17);
        CHECK_CPU_FEATURE(cpuId7, PCONFIG);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_EDX_19);
        CHECK_CPU_FEATURE(cpuId7, IBT);
        //CHECK_CPU_FEATURE(cpuId7, INDEX_7_EDX_21);
        CHECK_CPU_FEATURE(cpuId7, AMX_BF16);
        CHECK_CPU_FEATURE(cpuId7, AVX512_FP16);
        CHECK_CPU_FEATURE(cpuId7, AMX_TILE);
        CHECK_CPU_FEATURE(cpuId7, AMX_INT8);
        CHECK_CPU_FEATURE(cpuId7, IBRS_IBPB);
        CHECK_CPU_FEATURE(cpuId7, STIBP);
        CHECK_CPU_FEATURE(cpuId7, L1D_FLUSH);
        CHECK_CPU_FEATURE(cpuId7, ARCH_CAPABILITIES);
        CHECK_CPU_FEATURE(cpuId7, CORE_CAPABILITIES);
        CHECK_CPU_FEATURE(cpuId7, SSBD);
        CHECK_CPU_FEATURE(cpuId7, AVX_VNNI);
        CHECK_CPU_FEATURE(cpuId7, AVX512_BF16);
        CHECK_CPU_FEATURE(cpuId7, FZLRM);
        CHECK_CPU_FEATURE(cpuId7, FSRS);
        CHECK_CPU_FEATURE(cpuId7, FSRCS);
        CHECK_CPU_FEATURE(cpuId7, HRESET);
        CHECK_CPU_FEATURE(cpuId7, LAM);
    }

#undef CHECK_CPU_FEATURE

#elif defined(SYSTEM_ARCH_ARM64) || defined(SYSTEM_ARCH_ARM)

#endif
}

TEST_CASE("Macros", "[macros]")
{
    int x = 8;
    CHECK(SYSTEM_MACRO_CALL_OVERLOAD(TEST_MACRO_, 1) == 1);
    CHECK(SYSTEM_MACRO_CALL_OVERLOAD(TEST_MACRO_, 1, x) == 9);
    CHECK(SYSTEM_MACRO_CALL_OVERLOAD(TEST_MACRO_, 1, 2, x) == 11);
    CHECK(SYSTEM_MACRO_CALL_OVERLOAD(TEST_MACRO_, 1, 2, 3, 4) == 10);
    CHECK(SYSTEM_MACRO_CALL_OVERLOAD(TEST_MACRO_, x, 2, 3, 4, 5) == 22);
}

TEST_CASE("Guid", "[guid]")
{
    System::GuidData nullGuidData = {};
    System::Guid nullGuid;
    System::GuidData guid1Data {
        0x33221100,
        0x5544,
        0x7766,
        0x8899,
        0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, };
    System::Guid guid1("33221100-5544-7766-8899-AABBCCDDEEFF");

    CHECK(nullGuid.ToString() == "00000000-0000-0000-0000-000000000000");
    CHECK(guid1.ToString() == "33221100-5544-7766-8899-aabbccddeeff");
    CHECK(guid1.ToString(true) == "33221100-5544-7766-8899-AABBCCDDEEFF");
    CHECK(guid1.ToString() == "33221100-5544-7766-8899-aabbccddeeff");
    CHECK(guid1.ToString(true) == "33221100-5544-7766-8899-AABBCCDDEEFF");

    CHECK(memcmp(&nullGuid, &nullGuidData, sizeof(System::GuidData)) == 0);
    CHECK(memcmp(&guid1, &guid1Data, sizeof(System::GuidData)) == 0);

    {
        System::Guid guid(guid1);
        CHECK(memcmp(&guid, &guid1Data, sizeof(System::GuidData)) == 0);
    }
    {
        System::Guid guid(std::move(guid1));
        CHECK(memcmp(&guid, &guid1Data, sizeof(System::GuidData)) == 0);
    }

    guid1.Clear();
    CHECK(nullGuid == nullGuidData);
    
    // Check operators
    {
        System::GuidData smaller{ 0x00000001, 0x0002, 0x0003, 0x0004, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
        System::GuidData bigger{ 0x00000001, 0x0002, 0x0003, 0x0004, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };

        CHECK(smaller == smaller);

        CHECK(smaller == bigger);
        CHECK_FALSE(smaller != bigger);
        CHECK_FALSE(smaller < bigger);
        CHECK(smaller <= bigger);
        CHECK_FALSE(smaller > bigger);
        CHECK(smaller >= bigger);

        bigger.Dword = 2;
        CHECK_FALSE(smaller == bigger);
        CHECK(smaller != bigger);
        CHECK(smaller < bigger);
        CHECK(smaller <= bigger);
        CHECK_FALSE(smaller > bigger);
        CHECK_FALSE(smaller >= bigger);

        bigger.Dword = 1;
        bigger.Short1 = 3;
        CHECK_FALSE(smaller == bigger);
        CHECK(smaller != bigger);
        CHECK(smaller < bigger);
        CHECK(smaller <= bigger);
        CHECK_FALSE(smaller > bigger);
        CHECK_FALSE(smaller >= bigger);

        bigger.Short1 = 2;
        bigger.Short2 = 4;
        CHECK_FALSE(smaller == bigger);
        CHECK(smaller != bigger);
        CHECK(smaller < bigger);
        CHECK(smaller <= bigger);
        CHECK_FALSE(smaller > bigger);
        CHECK_FALSE(smaller >= bigger);
    }

    {
        System::Guid smaller("98e234e5-303e-75cc-b350-980d56978967");
        System::Guid bigger("a17b1eb8-797b-2c0a-8f30-03777bdf2add");

        CHECK_FALSE(smaller == bigger);
        CHECK(smaller != bigger);
        CHECK(smaller < bigger);
        CHECK(smaller <= bigger);
        CHECK_FALSE(smaller > bigger);
        CHECK_FALSE(smaller >= bigger);
    }

    // Map can use Guid as key
    std::map<System::Guid, std::string> map;
    map[System::GuidData{ 0x00000001, 0x0002, 0x0003, 0x0004, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a }] = "1";
    map[System::GuidData{ 0x00000002, 0x0002, 0x0003, 0x0004, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a }] = "2";

    CHECK(map[System::GuidData{ 0x00000001, 0x0002, 0x0003, 0x0004, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a }] == "1");
    CHECK(map[System::GuidData{ 0x00000002, 0x0002, 0x0003, 0x0004, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a }] == "2");
}

TEST_CASE("Environment variable manipulation", "[environment_variable]")
{
    CHECK(System::GetEnvVar("TestEnvVar") == "TestEnvVarValue");
    CHECK(System::SetEnvVar("TestEnvVar", "NewEnvVarValue") == true);
    CHECK(System::GetEnvVar("TestEnvVar") == "NewEnvVarValue");
    CHECK(System::UnsetEnvVar("TestEnvVar") == true);
    CHECK(System::GetEnvVar("TestEnvVar") == std::string());
}

#ifdef SYSTEM_DOTNET_TEST_ASSEMBLY
TEST_CASE("Load .Net assembly", "[load_dotnet]")
{
    struct TestClass
    {
        int X;
    };

    System::DotNet::DotNetCoreHost host;
    host.LoadDotNetCoreHost();
    auto entry_point = host.LoadAssemblyAndEntryPoint(
        "TestDotNetLoader.EntryPoint, TestDotNetLoader",
        "Main",
        SYSTEM_DOTNET_TEST_ASSEMBLY);

    auto x = entry_point(nullptr, 0);

    auto createTestClassFunction = host.GetFunctionDelegate<TestClass *(int32_t)>(
        "TestDotNetLoader.Class1, TestDotNetLoader",
        "CreateTestClass",
        "TestDotNetLoader.Class1+CreateTestClassDelegate, TestDotNetLoader");

    auto y = createTestClassFunction(50);
    CHECK(y->X == 50);
    y->X = 100;

    auto freeTestClassFunction = host.GetFunctionDelegate<void(TestClass*, int)>(
        "TestDotNetLoader.Class1, TestDotNetLoader",
        "FreeTestClass",
        "TestDotNetLoader.Class1+FreeTestClassDelegate, TestDotNetLoader");

    freeTestClassFunction(y, 100);
}
#endif

TEST_CASE("Set thread name", "[thread_name]")
{
    CHECK(System::SetCurrentThreadName("TestThreadName") == true);
}

TEST_CASE("String switch", "[string_switch]")
{
    {
        std::string string("string_hash1");
        std::string result;
        constexpr auto string_hash1 = System::StringSwitch::Hash("string_hash1");
        constexpr auto string_hash2 = System::StringSwitch::Hash("string_hash2");
        constexpr auto string_hash3 = System::StringSwitch::Hash("string_hash3");
        constexpr auto string_hash4 = System::StringSwitch::Hash("string_hash4");

        switch (System::StringSwitch::Hash(string))
        {
            case string_hash1: result = "string_hash1"; break;
            case string_hash2: result = "string_hash2"; break;
            case string_hash3: result = "string_hash3"; break;
            case string_hash4: result = "string_hash4"; break;
        }

        CHECK(result == string);

        string = "string_hash3";
        switch (System::StringSwitch::Hash(string))
        {
            case System::StringSwitch::Hash("string_hash1"): result = "string_hash1"; break;
            case System::StringSwitch::Hash("string_hash2"): result = "string_hash2"; break;
            case System::StringSwitch::Hash("string_hash3"): result = "string_hash3"; break;
            case System::StringSwitch::Hash("string_hash4"): result = "string_hash4"; break;
        }

        CHECK(result == string);
    }
    {
        std::wstring string(L"string_hash1");
        std::wstring result;
        constexpr auto string_hash1 = System::StringSwitch::Hash(L"string_hash1");
        constexpr auto string_hash2 = System::StringSwitch::Hash(L"string_hash2");
        constexpr auto string_hash3 = System::StringSwitch::Hash(L"string_hash3");
        constexpr auto string_hash4 = System::StringSwitch::Hash(L"string_hash4");

        switch (System::StringSwitch::Hash(string))
        {
            case string_hash1: result = L"string_hash1"; break;
            case string_hash2: result = L"string_hash2"; break;
            case string_hash3: result = L"string_hash3"; break;
            case string_hash4: result = L"string_hash4"; break;
        }

        CHECK(result == string);

        string = L"string_hash3";
        switch (System::StringSwitch::Hash(string))
        {
            case System::StringSwitch::Hash(L"string_hash1"): result = L"string_hash1"; break;
            case System::StringSwitch::Hash(L"string_hash2"): result = L"string_hash2"; break;
            case System::StringSwitch::Hash(L"string_hash3"): result = L"string_hash3"; break;
            case System::StringSwitch::Hash(L"string_hash4"): result = L"string_hash4"; break;
        }

        CHECK(result == string);
    }
    {// This behavior was not intended but it is convenient to tests char against wchar (on the ascii range)
        std::string string("string_hash1");
        std::string result;
        constexpr auto string_hash1 = System::StringSwitch::Hash(L"string_hash1");
        constexpr auto string_hash2 = System::StringSwitch::Hash(L"string_hash2");
        constexpr auto string_hash3 = System::StringSwitch::Hash(L"string_hash3");
        constexpr auto string_hash4 = System::StringSwitch::Hash(L"string_hash4");

        switch (System::StringSwitch::Hash(string))
        {
            case string_hash1: result = "string_hash1"; break;
            case string_hash2: result = "string_hash2"; break;
            case string_hash3: result = "string_hash3"; break;
            case string_hash4: result = "string_hash4"; break;
        }

        CHECK(result == string);

        string = "é";
        switch (System::StringSwitch::Hash(string))
        {
            case System::StringSwitch::Hash(L"é"): result = "string_hash1"; break;
            case System::StringSwitch::Hash("é") : result = "string_hash2"; break;
        }

        CHECK(result == "string_hash2");

        switch (System::StringSwitch::Hash(L"é"))
        {
            case System::StringSwitch::Hash(L"é"): result = "string_hash1"; break;
            case System::StringSwitch::Hash("é") : result = "string_hash2"; break;
        }

        CHECK(result == "string_hash1");

        string = u8"é";
        switch (System::StringSwitch::Hash(string))
        {
            case System::StringSwitch::Hash(L"é"): result = "string_hash1"; break;
            case System::StringSwitch::Hash(u8"é"): result = "string_hash2"; break;
        }

        CHECK(result == "string_hash2");
    }
}

TEST_CASE("Base64", "[base64]")
{
    CHECK(System::Encoding::Base64::Encode(R"({ "json_key": "json_value" })", true) == "eyAianNvbl9rZXkiOiAianNvbl92YWx1ZSIgfQ==");
    CHECK(System::Encoding::Base64::Encode(R"({ "json_key": "json_value" })", false) == "eyAianNvbl9rZXkiOiAianNvbl92YWx1ZSIgfQ");

    CHECK(System::Encoding::Base64::Decode("eyAianNvbl9rZXkiOiAianNvbl92YWx1ZSIgfQ==") == R"({ "json_key": "json_value" })");
    CHECK(System::Encoding::Base64::Decode("eyAianNvbl9rZXkiOiAianNvbl92YWx1ZSIgfQ") == R"({ "json_key": "json_value" })");

    CHECK(System::Encoding::Base64::Encode("\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb"    , true ) == "+/v7+/v7+/v7");
    CHECK(System::Encoding::Base64::Encode("\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb"    , false) == "+/v7+/v7+/v7");
    CHECK(System::Encoding::Base64::Encode("\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb", true ) == "+/v7+/v7+/v7+w==");
    CHECK(System::Encoding::Base64::Encode("\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb", false) == "+/v7+/v7+/v7+w");

    CHECK(System::Encoding::Base64::UrlEncode("\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb"    , true ) == "-_v7-_v7-_v7");
    CHECK(System::Encoding::Base64::UrlEncode("\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb"    , false) == "-_v7-_v7-_v7");
    CHECK(System::Encoding::Base64::UrlEncode("\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb", true ) == "-_v7-_v7-_v7-w==");
    CHECK(System::Encoding::Base64::UrlEncode("\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb", false) == "-_v7-_v7-_v7-w");
     
    CHECK(System::Encoding::Base64::Decode("+/v7+/v7+/v7")     == "\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb");
    CHECK(System::Encoding::Base64::Decode("+/v7+/v7+/v7+w==") == "\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb");
    CHECK(System::Encoding::Base64::Decode("+/v7+/v7+/v7+w")   == "\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb");

    CHECK(System::Encoding::Base64::UrlDecode("-_v7-_v7-_v7")     == "\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb");
    CHECK(System::Encoding::Base64::UrlDecode("-_v7-_v7-_v7-w==") == "\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb");
    CHECK(System::Encoding::Base64::UrlDecode("-_v7-_v7-_v7-w")   == "\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb\xfb");
}

inline std::ostream& operator<<(std::ostream& os, System::TranslatedMode mode)
{
    switch (mode)
    {
        case System::TranslatedMode::Unavailable: return os << "Unavailable";
        case System::TranslatedMode::Native     : return os << "Native";
        case System::TranslatedMode::Translated : return os << "Translated";
    }

    return os << "System::TranslatedMode(" << (int)mode << ')';
}

static void LoadLibraryCallback(std::string const& libraryName, void* libraryBase, System::Library::LoadLibraryReason reason, void* userParameter)
{
#if defined(SYSTEM_OS_WINDOWS)
    auto filename = System::Filesystem::Filename(libraryName);
    CHECK(filename == "shared.dll");
#elif defined(SYSTEM_OS_LINUX)
    auto filename = System::Filesystem::Filename(libraryName);
    CHECK(filename == "shared.so");
#elif defined(SYSTEM_OS_APPLE)
    auto filename = System::Filesystem::Filename(libraryName);
    CHECK(filename == "shared.dylib");
#endif
}

TEST_CASE("Load library", "[loadlibrary]")
{
    System::Library::Library shared;
    std::string lib_path = System::Filesystem::Join(System::Filesystem::Dirname(System::GetExecutablePath()), "shared");

    auto callbackToken = System::Library::AddLoadLibraryCallback(&LoadLibraryCallback, nullptr);

    shared.OpenLibrary(lib_path, true);
    std::cout << "From executable: " << std::endl
              << "  Parent pid             : " << System::GetParentProcessId() << std::endl
              << "  Executable pid         : " << System::GetProcessId() << std::endl
              << "  Operating System       : " << System::os_name << std::endl
              << "  System architecture    : " << System::arch_name << std::endl
              << "  Compiler               : " << System::compiler_name << std::endl
              << "  Translated mode        : " << System::GetTranslatedMode() << std::endl
              << "  Executable path        : " << System::GetExecutablePath() << std::endl
              << "  Executable module path : " << System::GetModulePath() << std::endl
              << "  Library module path    : " << shared.GetLibraryPath() << std::endl << std::endl;

    {
        auto sharedLibraryFunction = shared.GetSymbol<std::shared_ptr<std::string>()>("GetExecutablePath");
        CHECK(System::GetExecutablePath() == *sharedLibraryFunction());
    }

    {
        auto sharedLibraryFunction = shared.GetSymbol<std::shared_ptr<std::string>()>("GetModulePath");
        CHECK(shared.GetLibraryPath() == *sharedLibraryFunction());
    }

#if defined(SYSTEM_OS_WINDOWS)
    {
        auto tmp = System::Filesystem::Filename(System::GetExecutablePath());
        CHECK(tmp == "test_app.exe");
    }
    {
        auto tmp = System::Filesystem::Filename(System::GetModulePath());
        CHECK(tmp == "test_app.exe");
    }
    {
        auto tmp = System::Filesystem::Filename(shared.GetLibraryPath());
        CHECK(tmp == "shared.dll");
    }
    CHECK(System::Library::GetLibraryExtension() == ".dll");
#elif defined(SYSTEM_OS_LINUX)
    {
        auto tmp = System::Filesystem::Filename(System::GetExecutablePath());
        CHECK(tmp == "test_app");
    }
    {
        auto tmp = System::Filesystem::Filename(System::GetModulePath());
        CHECK(tmp == "test_app");
    }
    {
        auto tmp = System::Filesystem::Filename(shared.GetLibraryPath());
        CHECK(tmp == "shared.so");
    }
    CHECK(System::Library::GetLibraryExtension() == ".so");
#elif defined(SYSTEM_OS_APPLE)
    {
        auto tmp = System::Filesystem::Filename(System::GetExecutablePath());
        CHECK(tmp == "test_app");
    }
    {
        auto tmp = System::Filesystem::Filename(System::GetModulePath());
        CHECK(tmp == "test_app");
    }
    {
        auto tmp = System::Filesystem::Filename(shared.GetLibraryPath());
        CHECK(tmp == "shared.dylib");
    }
    CHECK(System::Library::GetLibraryExtension() == ".dylib");
#endif

    {
        auto tmp = System::Filesystem::Filename(shared.GetLibraryPath());
        CHECK(System::Library::GetLibraryHandle(tmp.c_str()) == shared.GetLibraryNativeHandle());
    }
    {
        CHECK(shared.GetLibraryPath() == System::Library::GetLibraryPath(shared.GetLibraryNativeHandle()));
    }

    System::Library::RemoveLoadLibraryCallback(callbackToken);
}

TEST_CASE("Show modules", "[showmodules]")
{
    std::cout << "Loaded modules: " << std::endl;
    auto modules = System::GetModules();
    for (auto const& item : modules)
    {
        std::cout << "  " << item << std::endl;
    }
}

TEST_CASE("List files", "[listfiles]")
{
    System::Filesystem::ListFiles("", true);
}

TEST_CASE("Dirname", "[dirname]")
{
    // Absolute path checks
#if defined(SYSTEM_OS_WINDOWS)
    CHECK(System::Filesystem::Dirname("D:\\test1\\test2\\//\\\\\\\\//test3\\") == "D:\\test1\\test2\\test3");
    CHECK(System::Filesystem::Dirname("D:\\test1\\test2/test3") == "D:\\test1\\test2");
    CHECK(System::Filesystem::Dirname("D:\\test1\\test2") == "D:\\test1");
    CHECK(System::Filesystem::Dirname("D:\\test1") == "D:");
    CHECK(System::Filesystem::Dirname("D:") == "");
#else
    CHECK(System::Filesystem::Dirname("/test1/test2\\//\\\\\\\\//test3\\") == "/test1/test2/test3");
    CHECK(System::Filesystem::Dirname("/test1/test2\\test3") == "/test1/test2");
    CHECK(System::Filesystem::Dirname("/test1/test2") == "/test1");
    CHECK(System::Filesystem::Dirname("/test1") == "/");
    CHECK(System::Filesystem::Dirname("/") == "");
#endif

    // Relative path checks
    CHECK(System::Filesystem::Dirname("test1/test2") == "test1");
    CHECK(System::Filesystem::Dirname("test1\\test2") == "test1");
    CHECK(System::Filesystem::Dirname("test1") == "");
    CHECK(System::Filesystem::Dirname("") == "");
}

TEST_CASE("Filename", "[filename]")
{
#if defined(SYSTEM_OS_WINDOWS)
    CHECK(System::Filesystem::Filename("D:\\dir\\file") == "file");
    CHECK(System::Filesystem::Filename("D:\\dir\\file\\") == "");
    CHECK(System::Filesystem::Filename("file") == "file");
#else
    CHECK(System::Filesystem::Filename("/dir/file") == "file");
    CHECK(System::Filesystem::Filename("/dir/file/") == "");
    CHECK(System::Filesystem::Filename("file") == "file");
#endif
}

TEST_CASE("Join", "[join]")
{
#if defined(SYSTEM_OS_WINDOWS)
    #define TSEP "\\"
#else
    #define TSEP "/"
#endif
    CHECK(System::Filesystem::Join("a", "b") == "a" TSEP "b");
    CHECK(System::Filesystem::Join("a\\", "b") == "a" TSEP "b");
    CHECK(System::Filesystem::Join("a", "\\b") == "a" TSEP "b");
    CHECK(System::Filesystem::Join("a/", "b") == "a" TSEP "b");
    CHECK(System::Filesystem::Join("a", "/b") == "a" TSEP "b");
    CHECK(System::Filesystem::Join("a") == "a");
    CHECK(System::Filesystem::Join("a", "b", "c") == "a" TSEP "b" TSEP "c");

#undef TSEP
}

TEST_CASE("Split string", "[SplitString]")
{
    auto array = System::String::SplitString("a,b,,c,d,", ',');

    CHECK(array.size() == 6);
    CHECK(array[0] == "a");
    CHECK(array[1] == "b");
    CHECK(array[2] == "");
    CHECK(array[3] == "c");
    CHECK(array[4] == "d");
    CHECK(array[5] == "");

    array = System::String::SplitString("aa", ',');
    CHECK(array.size() == 1);
    CHECK(array[0] == "aa");

    array = System::String::SplitString(",", ',');
    CHECK(array.size() == 2);
    CHECK(array[0] == "");
    CHECK(array[1] == "");
}

TEST_CASE("LeftTrim", "[left_trim]")
{
    // inplace std::string&
    {
        std::string test("  left trim  ");
        System::String::LeftTrim(test);
        CHECK(test == "left trim  ");
    }

    // copy char* (nullptr)
    {
        // don't crash
        std::string r = System::String::CopyLeftTrim(nullptr);
        CHECK(r == "");
    }
    // copy char*
    {
        std::string r = System::String::CopyLeftTrim("  left trim  ");
        CHECK(r == "left trim  ");
    }
    // copy std::string const&
    {
        std::string r = System::String::CopyLeftTrim(std::string("  left trim  "));
        CHECK(r == "left trim  ");
    }
    // copy std::string_view
    {
        std::string r = System::String::CopyLeftTrim(std::string_view{ "  left trim  " });
        CHECK(r == "left trim  ");
    }
}

TEST_CASE("RightTrim", "[right_trim]")
{
    // inplace std::string&
    {
        std::string test("  right trim  ");
        System::String::RightTrim(test);
        CHECK(test == "  right trim");
    }

    // copy char* (nullptr)
    {
        // don't crash
        std::string r = System::String::CopyRightTrim(nullptr);
        CHECK(r == "");
    }
    // copy char*
    {
        std::string r = System::String::CopyRightTrim("  right trim  ");
        CHECK(r == "  right trim");
    }
    // copy std::string const&
    {
        std::string r = System::String::CopyRightTrim(std::string("  right trim  "));
        CHECK(r == "  right trim");
    }
    // copy std::string_view
    {
        std::string r = System::String::CopyRightTrim(std::string_view("  right trim  "));
        CHECK(r == "  right trim");
    }
}

TEST_CASE("Trim", "[trim]")
{
    // inplace std::string&
    {
        std::string test("  both end trim  ");
        System::String::Trim(test);
        CHECK(test == "both end trim");
    }

    // copy char* (nullptr)
    {
        // don't crash
        std::string r = System::String::CopyTrim(nullptr);
        CHECK(r == "");
    }
    // copy char*
    {
        std::string r = System::String::CopyTrim("  both end trim  ");
        CHECK(r == "both end trim");
    }
    // copy std::string const&
    {
        std::string r = System::String::CopyTrim(std::string("  both end trim  "));
        CHECK(r == "both end trim");
    }
    // copy std::string_view
    {
        std::string r = System::String::CopyTrim(std::string_view("  both end trim  "));
        CHECK(r == "both end trim");
    }
}

TEST_CASE("ToLower", "[to_lower]")
{
    // inplace char* (nullptr)
    {
        // don't crash
        System::String::ToLower(nullptr);
    }
    // inplace char*
    {
        char buffer[] = "TO LOWER";
        System::String::ToLower(buffer);
        CHECK(strcmp(buffer, "to lower") == 0);
    }
    // inplace std::string&
    {
        std::string buffer = "TO LOWER";
        System::String::ToLower(buffer);
        CHECK(buffer == "to lower");
    }
    // copy const char*
    {
        char buffer[] = "TO LOWER";
        std::string result = System::String::CopyLower(buffer);
        CHECK(strcmp(buffer, "TO LOWER") == 0);
        CHECK(result == "to lower");
    }
    // copy std::string const&
    {
        std::string buffer = "TO LOWER";
        std::string result = System::String::CopyLower(buffer);
        CHECK(buffer == "TO LOWER");
        CHECK(result == "to lower");
    }
    // copy std::string_view
    {
        std::string_view buffer = "TO LOWER";
        std::string result = System::String::CopyLower(buffer);
        CHECK(strcmp(&buffer[0], "TO LOWER") == 0);
        CHECK(result == "to lower");
    }
}

TEST_CASE("ToUpper", "[to_upper]")
{
    // inplace char* (nullptr)
    {
        // don't crash
        System::String::ToUpper(nullptr);
    }
    // inplace char*
    {
        char buffer[] = "to upper";
        System::String::ToUpper(buffer);
        CHECK(strcmp(buffer, "TO UPPER") == 0);
    }
    // inplace std::string&
    {
        std::string buffer = "to upper";
        System::String::ToUpper(buffer);
        CHECK(buffer == "TO UPPER");
    }
    // copy const char*
    {
        char buffer[] = "to upper";
        std::string result = System::String::CopyUpper(buffer);
        CHECK(strcmp(buffer, "to upper") == 0);
        CHECK(result == "TO UPPER");
    }
    // copy std::string const&
    {
        std::string buffer = "to upper";
        std::string result = System::String::CopyUpper(buffer);
        CHECK(buffer == "to upper");
        CHECK(result == "TO UPPER");
    }
    // copy std::string_view
    {
        std::string_view buffer = "to upper";
        std::string result = System::String::CopyUpper(buffer);
        CHECK(strcmp(&buffer[0], "to upper") == 0);
        CHECK(result == "TO UPPER");
    }
}

TEST_CASE("CloneString", "[clone_string]")
{
    char* result = nullptr;

    // const char* (nullptr)
    {
        result = System::String::CloneString(nullptr);
        CHECK((result != nullptr && strcmp(result, "") == 0));
        delete[] result; result = nullptr;
    }
    // const char*
    {
        const char* tmp = "Clone test";
        result = System::String::CloneString(tmp);
        CHECK((result != nullptr && strcmp(result, "Clone test") == 0));
        delete[] result; result = nullptr;
    }
    // std::string const&
    {
        result = System::String::CloneString(std::string("Clone test"));
        CHECK((result != nullptr && strcmp(result, "Clone test") == 0));
        delete[] result; result = nullptr;
    }
    // std::string_view
    {
        result = System::String::CloneString(std::string_view("Clone test"));
        CHECK((result != nullptr && strcmp(result, "Clone test") == 0));
        delete[] result; result = nullptr;
    }
}

TEST_CASE("CopyString", "[copy_string]")
{
    size_t result;
    {
        char buffer[200];
        // const char* (nullptr)
        result = System::String::CopyString(nullptr, buffer, 200);
        CHECK((result == 0 && strcmp(buffer, "") == 0));
        memset(buffer, 0, 200);
        // const char*
        result = System::String::CopyString("Copy test", buffer, 200);
        CHECK((result == 9 && strcmp(buffer, "Copy test") == 0));
        memset(buffer, 0, 200);
        // std::string_view
        result = System::String::CopyString(std::string_view("Copy test sv"), buffer, 200);
        CHECK((result == 12 && strcmp(buffer, "Copy test sv") == 0));
        memset(buffer, 0, 200);
        // std::string const&
        result = System::String::CopyString(std::string("Copy test string"), buffer, 200);
        CHECK((result == 16 && strcmp(buffer, "Copy test string") == 0));
        memset(buffer, 0, 200);
    }
    {
        char buffer[5];
        // const char* (nullptr)
        result = System::String::CopyString(nullptr, buffer, 5);
        CHECK((result == 0 && strcmp(buffer, "") == 0));
        memset(buffer, 0, 5);
        // const char*
        result = System::String::CopyString("Copy test", buffer, 5);
        CHECK((result == 4 && strcmp(buffer, "Copy") == 0));
        memset(buffer, 0, 5);
        // std::string_view
        result = System::String::CopyString(std::string_view("Copy test sv"), buffer, 5);
        CHECK((result == 4 && strcmp(buffer, "Copy") == 0));
        memset(buffer, 0, 5);
        // std::string const&
        result = System::String::CopyString(std::string("Copy test string"), buffer, 5);
        CHECK((result == 4 && strcmp(buffer, "Copy") == 0));
        memset(buffer, 0, 5);
    }

    //
    {
        char buffer[200];
        // const char* (nullptr)
        result = System::String::CopyString(nullptr, buffer);
        CHECK((result == 0 && strcmp(buffer, "") == 0));
        memset(buffer, 0, 200);
        // const char*
        result = System::String::CopyString("Copy test", buffer);
        CHECK((result == 9 && strcmp(buffer, "Copy test") == 0));
        memset(buffer, 0, 200);
        // std::string_view
        result = System::String::CopyString(std::string_view("Copy test sv"), buffer);
        CHECK((result == 12 && strcmp(buffer, "Copy test sv") == 0));
        memset(buffer, 0, 200);
        // std::string const&
        result = System::String::CopyString(std::string("Copy test string"), buffer);
        CHECK((result == 16 && strcmp(buffer, "Copy test string") == 0));
        memset(buffer, 0, 200);
    }
    {
        char buffer[5];
        // const char* (nullptr)
        result = System::String::CopyString(nullptr, buffer);
        CHECK((result == 0 && strcmp(buffer, "") == 0));
        memset(buffer, 0, 5);
        // const char*
        result = System::String::CopyString("Copy test", buffer);
        CHECK((result == 4 && strcmp(buffer, "Copy") == 0));
        memset(buffer, 0, 5);
        // std::string_view
        result = System::String::CopyString(std::string_view("Copy test sv"), buffer);
        CHECK((result == 4 && strcmp(buffer, "Copy") == 0));
        memset(buffer, 0, 5);
        // std::string const&
        result = System::String::CopyString(std::string("Copy test string"), buffer);
        CHECK((result == 4 && strcmp(buffer, "Copy") == 0));
        memset(buffer, 0, 5);
    }
}
