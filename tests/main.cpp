#include <System/System.h>
#include <System/Library.h>
#include <System/Filesystem.h>
#include <System/SystemDetector.h>
#include <System/String.hpp>
#include <System/TypeName.hpp>
#include <System/Encoding.hpp>
#include <System/StringSwitch.hpp>

#include <iostream>

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, char *argv[])
{
    return Catch::Session().run(argc, argv);
}

TEST_CASE("Environment variable manipulation", "[environment_variable]")
{
    CHECK(System::GetEnvVar("TestEnvVar") == "TestEnvVarValue");
    CHECK(System::SetEnvVar("TestEnvVar", "NewEnvVarValue") == true);
    CHECK(System::GetEnvVar("TestEnvVar") == "NewEnvVarValue");
    CHECK(System::UnsetEnvVar("TestEnvVar") == true);
    CHECK(System::GetEnvVar("TestEnvVar") == std::string());
}

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

class TypeNameTestClass
{};

struct TypeNameTestStruct
{};

template<typename T>
struct XXXX
{};

TEST_CASE("Type name", "[TypeName]")
{
    {
        TypeNameTestClass  test1, *pointerTest1 = &test1, **pointerTest11 = &pointerTest1, ***pointerTest111 = &pointerTest11;
        TypeNameTestStruct test2, *pointerTest2 = &test2, **pointerTest22 = &pointerTest2, ***pointerTest222 = &pointerTest22;

#if defined(SYSTEM_OS_APPLE)
        CHECK(System::TypeName::TypeName<TypeNameTestClass>().to_string() == "TypeNameTestClass");
        CHECK(System::TypeName::TypeName<TypeNameTestClass*>().to_string() == "TypeNameTestClass *");
        CHECK(System::TypeName::TypeName<TypeNameTestClass**>().to_string() == "TypeNameTestClass **");
        CHECK(System::TypeName::TypeName<TypeNameTestClass&>().to_string() == "TypeNameTestClass &");
        CHECK(System::TypeName::TypeName<TypeNameTestClass&&>().to_string() == "TypeNameTestClass &&");
#else
        CHECK(System::TypeName::TypeName<TypeNameTestClass>().to_string() == "TypeNameTestClass");
        CHECK(System::TypeName::TypeName<TypeNameTestClass*>().to_string() == "TypeNameTestClass*");
        CHECK(System::TypeName::TypeName<TypeNameTestClass**>().to_string() == "TypeNameTestClass**");
        CHECK(System::TypeName::TypeName<TypeNameTestClass&>().to_string() == "TypeNameTestClass&");
        CHECK(System::TypeName::TypeName<TypeNameTestClass&&>().to_string() == "TypeNameTestClass&&");
#endif

        CHECK(System::TypeName::BaseTypeName(test1).to_string() == "TypeNameTestClass");
        CHECK(System::TypeName::BaseTypeName(pointerTest1).to_string() == "TypeNameTestClass");
        CHECK(System::TypeName::BaseTypeName(pointerTest11).to_string() == "TypeNameTestClass");
        CHECK(System::TypeName::BaseTypeName(pointerTest111).to_string() == "TypeNameTestClass");

#if defined(SYSTEM_OS_APPLE)
        CHECK(System::TypeName::TypeName<TypeNameTestStruct>().to_string() == "TypeNameTestStruct");
        CHECK(System::TypeName::TypeName<TypeNameTestStruct*>().to_string() == "TypeNameTestStruct *");
        CHECK(System::TypeName::TypeName<TypeNameTestStruct**>().to_string() == "TypeNameTestStruct **");
        CHECK(System::TypeName::TypeName<TypeNameTestStruct&>().to_string() == "TypeNameTestStruct &");
        CHECK(System::TypeName::TypeName<TypeNameTestStruct&&>().to_string() == "TypeNameTestStruct &&");
#else
        CHECK(System::TypeName::TypeName<TypeNameTestStruct>().to_string() == "TypeNameTestStruct");
        CHECK(System::TypeName::TypeName<TypeNameTestStruct*>().to_string() == "TypeNameTestStruct*");
        CHECK(System::TypeName::TypeName<TypeNameTestStruct**>().to_string() == "TypeNameTestStruct**");
        CHECK(System::TypeName::TypeName<TypeNameTestStruct&>().to_string() == "TypeNameTestStruct&");
        CHECK(System::TypeName::TypeName<TypeNameTestStruct&&>().to_string() == "TypeNameTestStruct&&");
#endif

        CHECK(System::TypeName::BaseTypeName(test2).to_string() == "TypeNameTestStruct");
        CHECK(System::TypeName::BaseTypeName(pointerTest2).to_string() == "TypeNameTestStruct");
        CHECK(System::TypeName::BaseTypeName(pointerTest22).to_string() == "TypeNameTestStruct");
        CHECK(System::TypeName::BaseTypeName(pointerTest222).to_string() == "TypeNameTestStruct");
    }
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

TEST_CASE("Load library", "[loadlibrary]")
{
    System::Library::Library shared;
    std::string lib_path = System::Filesystem::Join(System::Filesystem::Dirname(System::GetExecutablePath()), "shared");
    shared.OpenLibrary(lib_path, true);
    std::cout << "From executable: " << std::endl
              << "  Executable pid        : " << System::GetProcessId() << std::endl
              << "  Translated mode       : " << System::GetTranslatedMode() << std::endl
              << "  Executable path       : " << System::GetExecutablePath() << std::endl
              << "  Executable module path: " << System::GetModulePath() << std::endl
              << "  Library module path   : " << shared.GetLibraryPath() << std::endl << std::endl;

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
    // copy System::StringView
    {
        std::string r = System::String::CopyLeftTrim(System::StringView("  left trim  "));
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
    // copy System::StringView
    {
        std::string r = System::String::CopyRightTrim(System::StringView("  right trim  "));
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
    // copy System::StringView
    {
        std::string r = System::String::CopyTrim(System::StringView("  both end trim  "));
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
    // copy System::StringView
    {
        System::StringView buffer = "TO LOWER";
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
    // copy System::StringView
    {
        System::StringView buffer = "to upper";
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
    // System::StringView
    {
        result = System::String::CloneString(System::StringView("Clone test"));
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
        // System::StringView
        result = System::String::CopyString(System::StringView("Copy test sv"), buffer, 200);
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
        // System::StringView
        result = System::String::CopyString(System::StringView("Copy test sv"), buffer, 5);
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
        // System::StringView
        result = System::String::CopyString(System::StringView("Copy test sv"), buffer);
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
        // System::StringView
        result = System::String::CopyString(System::StringView("Copy test sv"), buffer);
        CHECK((result == 4 && strcmp(buffer, "Copy") == 0));
        memset(buffer, 0, 5);
        // std::string const&
        result = System::String::CopyString(std::string("Copy test string"), buffer);
        CHECK((result == 4 && strcmp(buffer, "Copy") == 0));
        memset(buffer, 0, 5);
    }
}