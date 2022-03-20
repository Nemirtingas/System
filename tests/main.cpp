#include <System/System.h>
#include <System/Library.h>
#include <System/Filesystem.h>
#include <System/SystemDetector.h>

#include <iostream>

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, char *argv[])
{
    return Catch::Session().run(argc, argv);
}

TEST_CASE("Load library", "[loadlibrary]")
{
    System::Library::Library shared;
    std::string lib_path = System::Filesystem::Join(System::Filesystem::Dirname(System::GetExecutablePath()), "shared");
    shared.OpenLibrary(lib_path, true);
    std::cout << "From executable: " << std::endl
        << "  Executable path       : " << System::GetExecutablePath() << std::endl
        << "  Executable module path: " << System::GetModulePath() << std::endl
        << "  Library module path   : " << shared.GetLibraryPath() << std::endl << std::endl;
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

TEST_CASE("Dirname", "[dirname]")
{
#if defined(SYSTEM_OS_WINDOWS)
    std::string p("D:\\test1\\test2\\//\\\\\\\\//test3\\");

    REQUIRE(p == "D:\\test1\\test2\\//\\\\\\\\//test3\\");

    p = System::Filesystem::Dirname(p);
    REQUIRE(p == "D:\\test1\\test2\\test3");

    p = System::Filesystem::Dirname(p);
    REQUIRE(p == "D:\\test1\\test2");

    p = System::Filesystem::Dirname(p);
    REQUIRE(p == "D:\\test1");

    p = System::Filesystem::Dirname(p);
    REQUIRE(p == "D:");

    p = System::Filesystem::Dirname(p);
    REQUIRE(p == "D:");

    p = "test1\\test2";
    p = System::Filesystem::Dirname(p);
    REQUIRE(p == "test1");

    p = System::Filesystem::Dirname(p);
    REQUIRE(p == "");

    p = System::Filesystem::Dirname(p);
    REQUIRE(p == "");

#else
    std::string p("/test1\\test2\\//\\\\\\\\//test3\\");

    REQUIRE(p == "/test1\\test2\\//\\\\\\\\//test3\\");

    p = System::Filesystem::Dirname(p);
    REQUIRE(p == "/test1/test2/test3");

    p = System::Filesystem::Dirname(p);
    REQUIRE(p == "/test1/test2");

    p = System::Filesystem::Dirname(p);
    REQUIRE(p == "/test1");

    p = System::Filesystem::Dirname(p);
    REQUIRE(p == "/");

    p = System::Filesystem::Dirname(p);
    REQUIRE(p == "/");

#endif
}

TEST_CASE("Filename", "[filename]")
{
#if defined(SYSTEM_OS_WINDOWS)
    REQUIRE(System::Filesystem::Filename("D:\\dir\\file") == "file");
    REQUIRE(System::Filesystem::Filename("D:\\dir\\file\\") == "");
    REQUIRE(System::Filesystem::Filename("file") == "file");
#else
    REQUIRE(System::Filesystem::Filename("/dir/file") == "file");
    REQUIRE(System::Filesystem::Filename("/dir/file/") == "");
    REQUIRE(System::Filesystem::Filename("file") == "file");
#endif
}