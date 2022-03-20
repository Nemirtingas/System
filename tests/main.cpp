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
    CHECK(System::Filesystem::Dirname("D:\\test1\\test2\\//\\\\\\\\//test3\\") == "D:\\test1\\test2\\test3");
    CHECK(System::Filesystem::Dirname("D:\\test1\\test2/test3") == "D:\\test1\\test2");
    CHECK(System::Filesystem::Dirname("D:\\test1\\test2") == "D:\\test1");
    CHECK(System::Filesystem::Dirname("D:\\test1") == "D:");
    CHECK(System::Filesystem::Dirname("D:") == "D:");

    CHECK(System::Filesystem::Dirname("test1/test2") == "test1");
    CHECK(System::Filesystem::Dirname("test1\\test2") == "test1");
    CHECK(System::Filesystem::Dirname("test1") == "");
    CHECK(System::Filesystem::Dirname("") == "");

#else
    CHECK(System::Filesystem::Dirname("/test1/test2\\//\\\\\\\\//test3\\") == "/test1/test2/test3");
    CHECK(System::Filesystem::Dirname("/test1/test2\\test3") == "/test1/test2");
    CHECK(System::Filesystem::Dirname("/test1/test2") == "/test1");
    CHECK(System::Filesystem::Dirname("/test1") == "/");
    CHECK(System::Filesystem::Dirname("/") == "/");

    CHECK(System::Filesystem::Dirname("test1/test2") == "test1");
    CHECK(System::Filesystem::Dirname("test1\\test2") == "test1");
    CHECK(System::Filesystem::Dirname("test1") == "");
    CHECK(System::Filesystem::Dirname("") == "");
#endif
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
    CHECK(System::Filesystem::Join("a", "b") == "a\\b");
    CHECK(System::Filesystem::Join("a\\", "b") == "a\\b");
    CHECK(System::Filesystem::Join("a", "\\b") == "a\\b");
    CHECK(System::Filesystem::Join("a/", "b") == "a\\b");
    CHECK(System::Filesystem::Join("a", "/b") == "a\\b");
    CHECK(System::Filesystem::Join("a") == "a");
    CHECK(System::Filesystem::Join("a", "b", "c") == "a\\b\\c");
#else
    CHECK(System::Filesystem::Join("a", "b") == "a/b");
    CHECK(System::Filesystem::Join("a\\", "b") == "a/b");
    CHECK(System::Filesystem::Join("a", "\\b") == "a/b");
    CHECK(System::Filesystem::Join("a/", "b") == "a/b");
    CHECK(System::Filesystem::Join("a", "/b") == "a/b");
    CHECK(System::Filesystem::Join("a") == "a");
    CHECK(System::Filesystem::Join("a", "b", "c") == "a/b/c");
#endif
}