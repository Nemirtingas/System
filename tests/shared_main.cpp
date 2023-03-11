#include <System/System.h>
#include <System/SystemDetector.h>
#include <System/SystemExports.h>

#include <iostream>

void shared_library_load()
{
    std::cout << "From library: " << std::endl
              << "  Executable pid         : " << System::GetProcessId() << std::endl
              << "  Library executable path: " << System::GetExecutablePath() << std::endl
              << "  Library module path    : " << System::GetModulePath() << std::endl << std::endl;
}

void shared_library_unload()
{
}

#if defined(SYSTEM_OS_WINDOWS)

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        shared_library_load();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        shared_library_unload();
        break;
    }
    return TRUE;
}

#elif defined(SYSTEM_OS_LINUX) || defined(SYSTEM_OS_APPLE)

__attribute__((constructor)) SYSTEM_HIDE_API(void, SYSTEM_CALL_DEFAULT) system_shared_library_constructor()
{
    shared_library_load();
}

__attribute__((destructor)) SYSTEM_HIDE_API(void, SYSTEM_CALL_DEFAULT) system_shared_library_destructor()
{
    shared_library_unload();
}

#endif