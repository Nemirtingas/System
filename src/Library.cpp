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

#include <System/Library.h>
#include <System/Encoding.hpp>
#include "System_internals.h"
#include <memory>

#if defined(SYSTEM_OS_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #define VC_EXTRALEAN
    #define NOMINMAX
    #include <Windows.h>

    constexpr char library_suffix[] = ".dll";

        typedef LONG NTSTATUS;

    typedef struct _UNICODE_STR
    {
        USHORT Length;
        USHORT MaximumLength;
        PWSTR pBuffer;
    } UNICODE_STR, * PUNICODE_STR;
    
    typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA {
        ULONG           Flags;             // Reserved.
        PUNICODE_STR FullDllName;       // The full path name of the DLL module.
        PUNICODE_STR BaseDllName;       // The base file name of the DLL module.
        PVOID           DllBase;           // A pointer to the base address for the DLL in memory.
        ULONG           SizeOfImage;       // The size of the DLL image, in bytes.
    } LDR_DLL_LOADED_NOTIFICATION_DATA, * PLDR_DLL_LOADED_NOTIFICATION_DATA;
    
    typedef struct _LDR_DLL_UNLOADED_NOTIFICATION_DATA {
        ULONG           Flags;             // Reserved.
        PUNICODE_STR FullDllName;       // The full path name of the DLL module.
        PUNICODE_STR BaseDllName;       // The base file name of the DLL module.
        PVOID           DllBase;           // A pointer to the base address for the DLL in memory.
        ULONG           SizeOfImage;       // The size of the DLL image, in bytes.
    } LDR_DLL_UNLOADED_NOTIFICATION_DATA, * PLDR_DLL_UNLOADED_NOTIFICATION_DATA;
    
    typedef union _LDR_DLL_NOTIFICATION_DATA {
        LDR_DLL_LOADED_NOTIFICATION_DATA   Loaded;
        LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
    } LDR_DLL_NOTIFICATION_DATA, * PLDR_DLL_NOTIFICATION_DATA;
    
    typedef VOID(CALLBACK* PLDR_DLL_NOTIFICATION_FUNCTION)(
        ULONG                       NotificationReason,
        PLDR_DLL_NOTIFICATION_DATA  NotificationData,
        PVOID                       Context);
    
    typedef struct _LDR_DLL_NOTIFICATION_ENTRY {
        LIST_ENTRY                     List;
        PLDR_DLL_NOTIFICATION_FUNCTION Callback;
        PVOID                          Context;
    } LDR_DLL_NOTIFICATION_ENTRY, * PLDR_DLL_NOTIFICATION_ENTRY;
    
    typedef NTSTATUS(NTAPI* _LdrRegisterDllNotification) (
        ULONG                          Flags,
        PLDR_DLL_NOTIFICATION_FUNCTION NotificationFunction,
        PVOID                          Context,
        PVOID* Cookie);
    
    typedef NTSTATUS(NTAPI* _LdrUnregisterDllNotification)(PVOID Cookie);

    #define LDR_DLL_NOTIFICATION_REASON_LOADED 1
    #define LDR_DLL_NOTIFICATION_REASON_UNLOADED 2

    struct LibraryNotificationParameter_t
    {
        void* Cookie;
        System::Library::LoadLibraryCallback_t Callback;
        void* UserParameter;
    };

#elif defined(SYSTEM_OS_LINUX) || defined(SYSTEM_OS_APPLE)
    #include <dlfcn.h>
    #include <cstring>

    #if defined(SYSTEM_OS_LINUX)
        #include <dirent.h> // to open directories
        #include <unistd.h>
        #include <sys/types.h>
        #include <sys/stat.h>

        constexpr char library_suffix[] = ".so";
    #else
        #include <mach-o/dyld_images.h>

        constexpr char library_suffix[] = ".dylib";
    #endif
#endif

namespace System {

namespace Library {

#if defined(SYSTEM_OS_WINDOWS)

void* OpenLibrary(const char* library_name)
{
    if (library_name == nullptr)
        return nullptr;

    std::wstring wide(System::Encoding::Utf8ToWChar(std::string_view(library_name)));
    return LoadLibraryW(wide.c_str());
}

void CloseLibrary(void* handle)
{
    if(handle != nullptr)
        FreeLibrary((HMODULE)handle);
}

void* GetSymbol(void* handle, const char* symbol_name)
{
    if (symbol_name == nullptr)
        return nullptr;

    return GetProcAddress((HMODULE)handle, symbol_name);
}

void* GetLibraryHandle(const char* library_name)
{
    if (library_name == nullptr)
        return nullptr;

    std::wstring wide(System::Encoding::Utf8ToWChar(std::string_view(library_name)));
    return GetModuleHandleW(wide.c_str());
}

std::string GetLibraryPath(void* handle)
{
    if (handle == nullptr)
        return std::string();

    std::wstring wpath(1024, L'\0');

    DWORD size;
    while ((size = GetModuleFileNameW((HMODULE)handle, &wpath[0], wpath.length())) == wpath.length())
    {
        wpath.resize(wpath.length() * 2);
    }
    
    wpath.resize(size);
    return System::Encoding::WCharToUtf8(wpath);
}

static void CALLBACK InternalLoadLibraryCallback(ULONG NotificationReason, PLDR_DLL_NOTIFICATION_DATA  NotificationData, PVOID Context)
{
    auto loaderParameter = reinterpret_cast<LibraryNotificationParameter_t*>(Context);

    if (NotificationReason == LDR_DLL_NOTIFICATION_REASON_LOADED)
    {
        auto libraryName = System::Encoding::WCharToUtf8(std::wstring_view(NotificationData->Loaded.FullDllName->pBuffer, NotificationData->Loaded.FullDllName->Length/sizeof(WCHAR)));
        loaderParameter->Callback(libraryName, NotificationData->Loaded.DllBase, System::Library::LoadLibraryReason::Loaded, loaderParameter->UserParameter);
    }
    else if (NotificationReason == LDR_DLL_NOTIFICATION_REASON_UNLOADED)
    {
        auto libraryName = System::Encoding::WCharToUtf8(std::wstring_view(NotificationData->Unloaded.FullDllName->pBuffer, NotificationData->Unloaded.FullDllName->Length / sizeof(WCHAR)));
        loaderParameter->Callback(libraryName, NotificationData->Unloaded.DllBase, System::Library::LoadLibraryReason::Unloaded, loaderParameter->UserParameter);
    }
}

void* AddLoadLibraryCallback(LoadLibraryCallback_t callback, void* userParameter)
{
    static _LdrRegisterDllNotification LdrRegisterDllNotification = nullptr;
    LibraryNotificationParameter_t* loaderParameter = nullptr;

    if (LdrRegisterDllNotification == nullptr)
    {
        auto ntdll = GetLibraryHandle("ntdll.dll");
        if (ntdll !=  nullptr)
            LdrRegisterDllNotification = (decltype(LdrRegisterDllNotification))GetSymbol(ntdll, "LdrRegisterDllNotification");
    }

    if (LdrRegisterDllNotification != nullptr)
    {
        loaderParameter = new LibraryNotificationParameter_t();
        if (loaderParameter != nullptr)
        {
            loaderParameter->UserParameter = userParameter;
            loaderParameter->Callback = callback;
            if (LdrRegisterDllNotification(0, &InternalLoadLibraryCallback, loaderParameter, &loaderParameter->Cookie) < 0)
            {
                delete loaderParameter;
                loaderParameter = nullptr;
            }
        }
    }

    return (void*)loaderParameter;
}

void RemoveLoadLibraryCallback(void* token)
{
    static _LdrUnregisterDllNotification LdrUnregisterDllNotification = nullptr;

    if (LdrUnregisterDllNotification == nullptr)
    {
        auto ntdll = GetLibraryHandle("ntdll.dll");
        if (ntdll != nullptr)
            LdrUnregisterDllNotification = (decltype(LdrUnregisterDllNotification))GetSymbol(ntdll, "LdrUnregisterDllNotification");
    }

    if (LdrUnregisterDllNotification != nullptr && token != nullptr)
    {
        auto loaderParameter = reinterpret_cast<LibraryNotificationParameter_t*>(token);
        LdrUnregisterDllNotification(loaderParameter->Cookie);
        delete loaderParameter;
    }
}

#elif defined(SYSTEM_OS_LINUX) || defined(SYSTEM_OS_APPLE)

void* OpenLibrary(const char* library_name)
{
    if (library_name == nullptr)
        return nullptr;

    return dlopen(library_name, RTLD_NOW);
}

void CloseLibrary(void* handle)
{
    if(handle != nullptr)
        dlclose(handle);
}

void* GetSymbol(void* handle, const char* symbol_name)
{
    if (handle == nullptr)
        return nullptr;

    return dlsym(handle, symbol_name);
}

#if defined(SYSTEM_OS_LINUX)
    std::string GetLibraryPath(void* handle)
    {
        if (handle == nullptr)
            return std::string();

        std::string const self("/proc/self/map_files/");
        DIR* dir;
        struct dirent* dir_entry;
        std::string file_path;
        std::string res;
        struct stat file_stat;

        dir = opendir(self.c_str());
        if (dir != nullptr)
        {
            while ((dir_entry = readdir(dir)) != nullptr)
            {
                file_path = (self + dir_entry->d_name);
                if (dir_entry->d_type != DT_LNK)
                {// Not a link
                    continue;
                }

                file_path = System::ExpandSymlink(file_path);
                if (stat(file_path.c_str(), &file_stat) != 0 || !S_ISREG(file_stat.st_mode))
                    continue;

                void* lib_handle = dlopen(file_path.c_str(), RTLD_NOW);
                if (lib_handle != nullptr)
                {// Don't increment ref_counter.
                    dlclose(lib_handle);
                    if (handle == lib_handle)
                    {
                        res = std::move(file_path);
                        break;
                    }
                }
            }

            closedir(dir);
        }

        return res;
    }

    void* GetLibraryHandle(const char* library_name)
    {
        if (library_name == nullptr)
            return nullptr;

        std::string const self("/proc/self/map_files/");
        DIR* dir;
        struct dirent* dir_entry;
        void* res = nullptr;
        size_t library_name_len = strlen(library_name);

        dir = opendir(self.c_str());
        if (dir != nullptr)
        {
            std::string file_path;
            while ((dir_entry = readdir(dir)) != nullptr)
            {
                bool found = false;
                file_path = (self + dir_entry->d_name);
                if (dir_entry->d_type != DT_LNK)
                {// Not a link
                    continue;
                }

                file_path = System::ExpandSymlink(file_path);

                found = file_path == library_name;
                if (!found)
                {
                    auto pos = file_path.rfind('/');
                    if (pos != std::string::npos)
                        found = strncmp(file_path.c_str() + pos + 1, library_name, library_name_len) == 0;
                }
                if (found)
                {
                    res = dlopen(file_path.c_str(), RTLD_NOW);
                    if (res != nullptr)
                    {// Like Windows' GetModuleHandle, we don't want to increment the ref counter
                        dlclose(res);
                    }
                    break;
                }
            }

            closedir(dir);
        }

        return res;
    }
#else

    std::string GetLibraryPath(void* handle)
    {
        if (handle == nullptr)
            return std::string();

        task_dyld_info dyld_info;
        task_t t;
        pid_t pid = getpid();
        task_for_pid(mach_task_self(), pid, &t);
        mach_msg_type_number_t count = TASK_DYLD_INFO_COUNT;
        
        if (task_info(t, TASK_DYLD_INFO, reinterpret_cast<task_info_t>(&dyld_info), &count) == KERN_SUCCESS)
        {
            dyld_all_image_infos* dyld_img_infos = reinterpret_cast<dyld_all_image_infos*>(dyld_info.all_image_info_addr);
            for (int i = 0; i < dyld_img_infos->infoArrayCount; ++i)
            {
                void* res = dlopen(dyld_img_infos->infoArray[i].imageFilePath, RTLD_NOW);
                if (res != nullptr)
                {
                    dlclose(res);
                    if(res == handle)
                        return std::string(dyld_img_infos->infoArray[i].imageFilePath);
                }
            }
        }
        
        return std::string();
    }

    void* GetLibraryHandle(const char* library_name)
    {
        if (library_name == nullptr)
            return nullptr;

        void* res = nullptr;

        size_t library_name_len = strlen(library_name);

        task_dyld_info dyld_info;
        task_t t;
        pid_t pid = getpid();
        task_for_pid(mach_task_self(), pid, &t);
        mach_msg_type_number_t count = TASK_DYLD_INFO_COUNT;

        if (task_info(t, TASK_DYLD_INFO, reinterpret_cast<task_info_t>(&dyld_info), &count) == KERN_SUCCESS)
        {
            const char* pos;
            dyld_all_image_infos* dyld_img_infos = reinterpret_cast<dyld_all_image_infos*>(dyld_info.all_image_info_addr);
            for (int i = 0; i < dyld_img_infos->infoArrayCount; ++i)
            {
                pos = strrchr(dyld_img_infos->infoArray[i].imageFilePath, '/');
                if (pos != nullptr)
                {
                    ++pos;
                    if (strncmp(pos, library_name, library_name_len) == 0)
                    {
                        res = dlopen(dyld_img_infos->infoArray[i].imageFilePath, RTLD_NOW);
                        if (res != nullptr)
                        {// Like Windows' GetModuleHandle, we don't want to increment the ref counter
                            dlclose(res);
                        }
                        break;
                    }
                }
            }
        }

        return res;
    }
#endif

void* AddLoadLibraryCallback(LoadLibraryCallback_t callback, void* userParameter)
{
    return nullptr;
}

void RemoveLoadLibraryCallback(void* token)
{
}

#endif

std::string GetLibraryExtension()
{
    return std::string{ library_suffix };
}

class LibraryImpl
{
    std::shared_ptr<void> _Handle;

    struct LibraryHandleDestructor
    {
        void operator()(void* h)
        {
            System::Library::CloseLibrary(h);
        }
    };

public:
    inline bool OpenLibrary(std::string const& library_name, bool append_extension)
    {
        std::string lib_name = (append_extension ? library_name + library_suffix : library_name);

        void* lib = System::Library::OpenLibrary(lib_name.c_str());

        if (lib == nullptr)
        {
            lib_name = "lib" + lib_name;
            lib = System::Library::OpenLibrary(lib_name.c_str());

            if (lib == nullptr)
                return false;
        }

        _Handle = std::shared_ptr<void>(lib, LibraryHandleDestructor());
        return true;
    }

    inline void CloseLibrary()
    {
        _Handle.reset();
    }

    inline void* GetVSymbol(std::string const& symbol_name) const
    {
        return System::Library::GetSymbol(_Handle.get(), symbol_name.c_str());
    }

    inline std::string GetLibraryPath() const
    {
        return System::Library::GetLibraryPath(_Handle.get());
    }

    inline void* GetLibraryNativeHandle() const
    {
        return _Handle.get();
    }

    inline bool IsLoaded() const
    {
        return _Handle != nullptr;
    }
};

Library::Library():
    _Impl(new LibraryImpl)
{}

Library::Library(Library const& other):
    _Impl(new LibraryImpl(*other._Impl))
{}

Library::Library(Library&& other) noexcept:
    _Impl(other._Impl)
{
    other._Impl = nullptr;
}

Library& Library::operator=(Library const& other)
{
    *_Impl = *other._Impl;
    return *this;
}

Library& Library::operator=(Library&& other) noexcept
{
    std::swap(_Impl, other._Impl);
    return *this;
}

Library::~Library()
{
    delete _Impl; _Impl = nullptr;
}

bool Library::OpenLibrary(std::string const& library_name, bool append_extension)
{
    return _Impl->OpenLibrary(library_name, append_extension);
}

void Library::CloseLibrary()
{
    _Impl->CloseLibrary();
}

void* Library::GetVSymbol(std::string const& symbol_name) const
{
    return _Impl->GetVSymbol(symbol_name);
}

std::string Library::GetLibraryPath() const
{
    return _Impl->GetLibraryPath();
}

void* Library::GetLibraryNativeHandle() const
{
    return _Impl->GetLibraryNativeHandle();
}

bool Library::IsLoaded() const
{
    return _Impl->IsLoaded();
}

}

}
