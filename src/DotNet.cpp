#include <System/DotNet.hpp>
#include <System/Encoding.hpp>
#include <System/Library.h>
#include <System/Filesystem.h>
#include <System/System.h>
#include <System/ConstExpressions.hpp>
#include "System_internals.h"

#include <memory>
#include <unordered_map>
#include <string.h>

#if defined(SYSTEM_OS_WINDOWS)
#include "WinRegister.h"
#include <Windows.h>

#define UTF8_TO_HOSTFXR(str) System::Encoding::Utf8ToWChar(str)
#define HOSTFXR_STRING(str) L##str

#define DOTNET_NETHOST_NAME "nethost.dll"
#define DOTNET_HOSTFXR_NAME "hostfxr.dll"

#define HOSTFXR_CALLTYPE __cdecl
typedef wchar_t char_t;

#elif defined(SYSTEM_OS_LINUX)

#define UTF8_TO_HOSTFXR(str) str
#define HOSTFXR_STRING(str) str

#define DOTNET_NETHOST_NAME "libnethost.so"
#define DOTNET_HOSTFXR_NAME "libhostfxr.so"

#define HOSTFXR_CALLTYPE
typedef char char_t;

#elif defined(SYSTEM_OS_APPLE)

#define UTF8_TO_HOSTFXR(str) str
#define HOSTFXR_STRING(str) str

#define DOTNET_NETHOST_NAME "libnethost.dylib"
#define DOTNET_HOSTFXR_NAME "libhostfxr.dylib"

#define HOSTFXR_CALLTYPE
typedef char char_t;

#else

#error "Unknown OS"

#endif

#if defined(SYSTEM_ARCH_X64)
#define DOTNET_ARCH "x64"
#elif defined(SYSTEM_ARCH_X86)
#define DOTNET_ARCH "x86"
#elif defined(SYSTEM_ARCH_ARM)
#define DOTNET_ARCH "arm"
#elif defined(SYSTEM_ARCH_ARM64)
#define DOTNET_ARCH "arm64"
#endif

#define DOTNET_APPHOST_PACKS_DIRECTORY "packs"
#define DOTNET_APPHOST_DIRECTORY_BEGIN "Microsoft.NETCore.App.Host."
#define DOTNET_APPHOST_DIRECTORY_END   "-" DOTNET_ARCH

typedef void* hostfxr_handle;
struct hostfxr_initialize_parameters
{
    size_t size;
    const char_t* host_path;
    const char_t* dotnet_root;
};

enum hostfxr_delegate_type
{
    hdt_com_activation,
    hdt_load_in_memory_assembly,
    hdt_winrt_activation,
    hdt_com_register,
    hdt_com_unregister,
    hdt_load_assembly_and_get_function_pointer,
    hdt_get_function_pointer,
    hdt_load_assembly,
    hdt_load_assembly_bytes,
};

using get_hostfxr_path_fn = int(char_t* buffer, size_t* buffer_size, const struct get_hostfxr_parameters* parameters);

typedef int32_t(HOSTFXR_CALLTYPE* hostfxr_initialize_for_runtime_config_fn)(
    const char_t* runtime_config_path,
    const struct hostfxr_initialize_parameters* parameters,
    hostfxr_handle* host_context_handle);

typedef int32_t(HOSTFXR_CALLTYPE* hostfxr_get_runtime_delegate_fn)(
    const hostfxr_handle host_context_handle,
    enum hostfxr_delegate_type type,
    void** delegate);

typedef int32_t(HOSTFXR_CALLTYPE* hostfxr_close_fn)(const hostfxr_handle host_context_handle);

// Signature of delegate returned by coreclr_delegate_type::load_assembly_and_get_function_pointer
typedef int (SYSTEM_CORECLR_DELEGATE_CALLTYPE* load_assembly_and_get_function_pointer_fn)(
    const char_t* assembly_path      /* Fully qualified path to assembly */,
    const char_t* type_name          /* Assembly qualified type name */,
    const char_t* method_name        /* Public static method name compatible with delegateType */,
    const char_t* delegate_type_name /* Assembly qualified delegate type name or null
                                        or UNMANAGEDCALLERSONLY_METHOD if the method is marked with
                                        the UnmanagedCallersOnlyAttribute. */,
    void* reserved           /* Extensibility parameter (currently unused and must be 0) */,
    /*out*/ void** delegate          /* Pointer where to store the function pointer result */);

typedef int (SYSTEM_CORECLR_DELEGATE_CALLTYPE* get_function_pointer_fn)(
    const char_t* type_name          /* Assembly qualified type name */,
    const char_t* method_name        /* Public static method name compatible with delegateType */,
    const char_t* delegate_type_name /* Assembly qualified delegate type name or null,
                                        or UNMANAGEDCALLERSONLY_METHOD if the method is marked with
                                        the UnmanagedCallersOnlyAttribute. */,
    void* load_context       /* Extensibility parameter (currently unused and must be 0) */,
    void* reserved           /* Extensibility parameter (currently unused and must be 0) */,
    /*out*/ void** delegate          /* Pointer where to store the function pointer result */);

// Signature of delegate returned by load_assembly_and_get_function_pointer_fn when delegate_type_name == null (default)
typedef int (SYSTEM_CORECLR_DELEGATE_CALLTYPE* component_entry_point_fn)(void* arg, int32_t arg_size_in_bytes);

struct dotnet_version_t
{
    int major = 0;
    int minor = 0;
    int revision = 0;

    static dotnet_version_t from_string(std::string const& str)
    {
        if (str.empty())
        {
            return dotnet_version_t{};
        }
        
        // Parse string version
        const char* pVersion = str.c_str();

        uint16_t v[3] = {};

        for (int i = 0; i < 3; ++i)
        {
            v[i] = atoi(pVersion);
            pVersion = strchr(pVersion, '.');
            if (pVersion == nullptr)
                break;

            ++pVersion;
        }

        return { v[0], v[1], v[2] };
    }

    inline std::string to_string() const
    {
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(revision);
    }

    inline bool is_empty() const
    {
        return major == 0 && minor == 0 && revision == 0;
    }

    inline bool operator==(dotnet_version_t const& other) const
    {
        return major == other.major && minor == other.major && revision == other.revision;
    }

    inline bool operator<(dotnet_version_t const& other) const
    {
        return major < other.major || minor < other.minor || revision < other.revision;
    }
};

#if defined(SYSTEM_OS_WINDOWS)

static inline std::string TryRegistryKey(void* registry_root, const char* registry_key, const char* registry_value, uint32_t rights)
{
    auto key = RegistryManipulator::Key::OpenRootKey(registry_root, registry_key, rights, false);
    if (key.IsOpen())
    {
        auto v = key.ReadValue(registry_value);
        if (v.IsString())
            return System::Encoding::WCharToUtf8(v.GetString());
    }

    return std::string();
}

static std::string FindDotNetRoot()
{
    std::string dotnet_root;
    struct {
        void* root;
        const char* key;
        const char* value;
        uint32_t rights;
    } registry_tries[] = {
        (void*)RegistryManipulator::Roots::local_machine, "SOFTWARE\\dotnet\\Setup\\InstalledVersions\\" DOTNET_ARCH, "InstallLocation", RegistryManipulator::Rights::read,
        (void*)RegistryManipulator::Roots::local_machine, "SOFTWARE\\dotnet\\Setup\\InstalledVersions\\" DOTNET_ARCH, "InstallLocation", RegistryManipulator::Rights::read | RegistryManipulator::Rights::wow64_32key,
        (void*)RegistryManipulator::Roots::local_machine, "SOFTWARE\\dotnet\\Setup\\InstalledVersions\\" DOTNET_ARCH "\\sharedhost", "Path", RegistryManipulator::Rights::read
    };

    for (auto reg_try : registry_tries)
    {
        dotnet_root = TryRegistryKey(reg_try.root, reg_try.key, reg_try.value, reg_try.rights);
        if (!dotnet_root.empty())
            break;
    }

    //if (dotnet_root.empty())
    //{
    //    auto system_drive = System::GetEnvVar("SystemDrive");
    //    if (!system_drive.empty())
    //    {
    //        System::Filesystem::Join(system_drive, "Program Files", "dotnet");
    //        System::Filesystem::Join(system_drive, "Program Files (x86)", "dotnet");
    //        if (System::Filesystem::Exists(system_drive))
    //        {
    //
    //        }
    //    }
    //}

    return dotnet_root;
}

#elif defined(SYSTEM_OS_LINUX)

static inline std::string FindDotNetRoot()
{
    std::string dotnet_root = System::GetEnvVar("DOTNET_ROOT");
    if (dotnet_root.empty() && System::Filesystem::Exists("/usr/local/share/dotnet/"))
        dotnet_root = "/usr/local/share/dotnet/";
    if (dotnet_root.empty() && System::Filesystem::Exists("/usr/lib/dotnet/"))
        dotnet_root = "/usr/lib/dotnet/";

    return dotnet_root;
}

#elif defined(SYSTEM_OS_APPLE)

// /usr/local/share/dotnet/host/fxr/6.0.8/
// /usr/local/share/dotnet/packs/Microsoft.NETCore.App.Host.osx-x64/5.0.15/runtimes/osx-x64/native/

static inline std::string FindDotNetRoot()
{
    std::string dotnet_root = System::GetEnvVar("DOTNET_ROOT");
    if (dotnet_root.empty() && System::Filesystem::Exists("/usr/local/share/dotnet/"))
        dotnet_root = "/usr/local/share/dotnet/";

    return dotnet_root;
}

#endif

static std::pair<std::string, std::string> FindDotNetAppHostPath(std::string const& dotnet_root)
{
    auto packs_directory = System::Filesystem::Join(dotnet_root, DOTNET_APPHOST_PACKS_DIRECTORY);
    auto files = System::Filesystem::ListFiles(packs_directory, false, false);

    constexpr size_t apphost_directory_begin_size = System::ConstExpr::StrLen(DOTNET_APPHOST_DIRECTORY_BEGIN);
    constexpr size_t apphost_directory_end_size = System::ConstExpr::StrLen(DOTNET_APPHOST_DIRECTORY_END);
    constexpr size_t apphost_directory_min_size = apphost_directory_begin_size + apphost_directory_end_size + 1;

    for (auto const& file : files)
    {
        if (file.length() <= apphost_directory_min_size)
            continue;

        if (strncmp(file.c_str(), DOTNET_APPHOST_DIRECTORY_BEGIN, apphost_directory_begin_size) == 0 &&
            strncmp(file.c_str() + file.length() - apphost_directory_end_size, DOTNET_APPHOST_DIRECTORY_END, apphost_directory_end_size) == 0)
        {
            return { file, file.substr(apphost_directory_begin_size, file.length() - apphost_directory_begin_size) };
        }
    }

    return { std::string{}, std::string{} };
}

static std::string FindDotNetNetHostPath(std::string const& dotnet_root, dotnet_version_t wanted_version)
{
    std::string dotnet_host_directory;
    std::string dotnet_host_id;
    {
        auto result = FindDotNetAppHostPath(dotnet_root);
        dotnet_host_directory = std::move(result.first);
        dotnet_host_id = std::move(result.second);
    }
    if (dotnet_host_directory.empty())
        return std::string{};

    auto dotnet_host_path = System::Filesystem::Join(dotnet_root, "packs", dotnet_host_directory);
    auto versions = System::Filesystem::ListFiles(dotnet_host_path, false, false);
    dotnet_version_t found_version;

    for (auto const& version : versions)
    {
        auto dotnet_version = dotnet_version_t::from_string(version);
        if (!dotnet_version.is_empty())
        {
            if (wanted_version.is_empty())
            {
                if (found_version < dotnet_version)
                    found_version = dotnet_version;
            }
            else if (wanted_version == dotnet_version)
            {
                found_version = dotnet_version;
                break;
            }
        }
    }

    if (!found_version.is_empty())
    {
        auto nethost_library = System::Filesystem::Join(dotnet_host_path, found_version.to_string(), "runtimes", dotnet_host_id, "native", DOTNET_NETHOST_NAME);
        if (System::Filesystem::Exists(nethost_library))
            return nethost_library;
    }

    return std::string();
}

static std::string FindDotNetHostFxrPath(std::string const& dotnet_root, dotnet_version_t wanted_version)
{
    auto dotnet_host_path = System::Filesystem::Join(dotnet_root, "host", "fxr");
    auto versions = System::Filesystem::ListFiles(dotnet_host_path, false, false);
    dotnet_version_t found_version;

    for (auto const& version : versions)
    {
        auto dotnet_version = dotnet_version_t::from_string(version);
        if (!dotnet_version.is_empty())
        {
            if (wanted_version.is_empty())
            {
                if (found_version < dotnet_version)
                    found_version = dotnet_version;
            }
            else if (wanted_version == dotnet_version)
            {
                found_version == dotnet_version;
                break;
            }
        }
    }

    if (!found_version.is_empty())
    {
        auto nethost_library = System::Filesystem::Join(dotnet_host_path, found_version.to_string(), DOTNET_HOSTFXR_NAME);
        if (System::Filesystem::Exists(nethost_library))
            return nethost_library;
    }

    return std::string();
}

static std::string ResolveDotNetHostFxrPath(std::string dotnet_root, std::string const& dotnet_version)
{
    std::string host_fxr_library_path;

    if (dotnet_root.empty())
        dotnet_root = FindDotNetRoot();

    if (dotnet_root.empty())
        return host_fxr_library_path;

    auto wanted_version = dotnet_version_t::from_string(dotnet_version);
    auto dotnet_nethost_library = FindDotNetNetHostPath(dotnet_root, wanted_version);

    // Prefer to use the nethost library
    if (!dotnet_nethost_library.empty())
    {
        System::Library::Library hostLibrary;
        hostLibrary.OpenLibrary(dotnet_nethost_library, false);

        auto get_hostfxr_path = hostLibrary.GetSymbol<get_hostfxr_path_fn>("get_hostfxr_path");
        if (get_hostfxr_path != nullptr)
        {
            char_t buffer[1024];
            size_t bufferSize = 1024;

            if (get_hostfxr_path(buffer, &bufferSize, nullptr) == 0)
            {
#if defined(SYSTEM_OS_WINDOWS)
                host_fxr_library_path = System::Encoding::WCharToUtf8(buffer);
#else
                host_fxr_library_path = buffer;
#endif
            }
        }
    }

    if (host_fxr_library_path.empty())
        host_fxr_library_path = FindDotNetHostFxrPath(dotnet_root, wanted_version);

    return host_fxr_library_path;
}

namespace System {
namespace DotNet {

struct DotNetCoreAssembly
{
    std::shared_ptr<hostfxr_handle> HostHandle;
    std::string AssemblyPath;
    std::string AssemblyConfig;
};

class DotNetCoreHostImpl
{
    hostfxr_initialize_for_runtime_config_fn _HostfxrInitializeForRuntimeConfig;
    hostfxr_get_runtime_delegate_fn _HostfxrGetRuntimeDelegate;
    hostfxr_close_fn _HostfxrClose;
    System::Library::Library _HostfxrLibrary;

    std::unordered_map<std::string, DotNetCoreAssembly> _Assemblies;

    std::string _GetAssemblyName(std::string const& assembly_entry_point_class)
    {
        auto pos = assembly_entry_point_class.rfind(',');
        if (pos == std::string::npos)
            return std::string();

        ++pos;
        while (assembly_entry_point_class[pos] == ' ')
            ++pos;

        return assembly_entry_point_class.substr(pos);
    }

public:
    load_assembly_and_get_function_pointer_fn GetLoadAssemblyFunction(std::string const& assembly_name, std::string const& assembly_path, std::string config_path, hostfxr_delegate_type load_type) {
        if (assembly_name.empty())
            return nullptr;

        if (config_path.empty())
        {
            auto pos = assembly_path.rfind(".dll");
            if (pos == std::string::npos || pos != (assembly_path.length() - 4))
                return nullptr;

            config_path = assembly_path.substr(0, pos) + ".runtimeconfig.json";
        }

        load_assembly_and_get_function_pointer_fn loadAssemblyFunction = nullptr;

        hostfxr_handle ctx = nullptr;

        int32_t result = _HostfxrInitializeForRuntimeConfig(UTF8_TO_HOSTFXR(config_path).c_str(), nullptr, &ctx);
        if (ctx == nullptr)
            return nullptr;

        std::shared_ptr<hostfxr_handle> handle((hostfxr_handle*)ctx, [this](hostfxr_handle* ctx) { _HostfxrClose((hostfxr_handle)ctx); });

        if (result > 2)
            return nullptr;

        result = _HostfxrGetRuntimeDelegate(
            ctx,
            load_type,
            (void**)&loadAssemblyFunction
        );

        if (result != 0 || loadAssemblyFunction == nullptr)
            return nullptr;

        auto& assembly = _Assemblies[assembly_name];
        assembly.HostHandle = handle;
        assembly.AssemblyPath = assembly_path;
        assembly.AssemblyConfig = config_path;
        return loadAssemblyFunction;
    }

    get_function_pointer_fn GetAssemblyFunctionLoader(std::string const& assembly_entry_point_class)
    {
        auto assembly_name = _GetAssemblyName(assembly_entry_point_class);
        if (assembly_name.empty())
            return nullptr;

        auto it = _Assemblies.find(assembly_name);
        if (it == _Assemblies.end())
            return nullptr;

        get_function_pointer_fn loadAssemblyFunction;
        auto result = _HostfxrGetRuntimeDelegate(
            (hostfxr_handle)it->second.HostHandle.get(),
            hostfxr_delegate_type::hdt_get_function_pointer,
            (void**)&loadAssemblyFunction
        );

        if (result != 0)
            return nullptr;

        return loadAssemblyFunction;
    }

    bool LoadDotNetCoreHost(std::string const& dotnet_root, std::string const& dotnet_version)
    {
        auto host_fxr_library_path = ResolveDotNetHostFxrPath(dotnet_root, dotnet_version);
        if (host_fxr_library_path.empty())
            return false;

        if (!_HostfxrLibrary.OpenLibrary(host_fxr_library_path, false))
            return false;

        _HostfxrInitializeForRuntimeConfig = (hostfxr_initialize_for_runtime_config_fn)_HostfxrLibrary.GetVSymbol("hostfxr_initialize_for_runtime_config");
        _HostfxrGetRuntimeDelegate = (hostfxr_get_runtime_delegate_fn)_HostfxrLibrary.GetVSymbol("hostfxr_get_runtime_delegate");
        _HostfxrClose = (hostfxr_close_fn)_HostfxrLibrary.GetVSymbol("hostfxr_close");

        if (_HostfxrInitializeForRuntimeConfig == nullptr ||
            _HostfxrGetRuntimeDelegate == nullptr ||
            _HostfxrClose == nullptr)
        {
            _HostfxrLibrary.CloseLibrary();
            _HostfxrInitializeForRuntimeConfig = nullptr;
            _HostfxrGetRuntimeDelegate = nullptr;
            _HostfxrClose = nullptr;
            return false;
        }
        
        return true;
    }

    //bool LoadAssembly(std::string const& assembly_name, std::string const& assembly_path, std::string assembly_runtimeconfig)
    //{
    //    auto load_assembly_function = GetLoadAssemblyFunction(assembly_name, assembly_path, assembly_runtimeconfig, hostfxr_delegate_type::hdt_load_assembly);
    //    if (load_assembly_function == nullptr)
    //        return false;
    //
    //    return load_assembly_function(UTF8_TO_HOSTFXR(assembly_path).c_str(), nullptr, nullptr, nullptr, nullptr, nullptr) == 0;
    //}

    DefaultDelegateType LoadAssemblyAndEntryPoint(std::string const& assembly_entry_point_class, std::string const& assembly_entry_point_function, std::string const& assembly_path, std::string const& assembly_runtimeconfig)
    {
        auto assembly_name = _GetAssemblyName(assembly_entry_point_class);
        auto load_assembly_function = GetLoadAssemblyFunction(assembly_name, assembly_path, assembly_runtimeconfig, hostfxr_delegate_type::hdt_load_assembly_and_get_function_pointer);
        if (load_assembly_function == nullptr)
            return nullptr;

        component_entry_point_fn entryPoint = nullptr;

        load_assembly_function(
            UTF8_TO_HOSTFXR(assembly_path).c_str(),
            UTF8_TO_HOSTFXR(assembly_entry_point_class).c_str(),
            UTF8_TO_HOSTFXR(assembly_entry_point_function).c_str(),
            nullptr,
            nullptr,
            (void**)&entryPoint);

        return entryPoint;
    }

    void* GetFunctionDelegate(std::string const& full_qualified_class_name, std::string const& static_public_function_name, std::string const& full_qualifed_delegate_type)
    {
        auto assembly_name = _GetAssemblyName(full_qualified_class_name);
        if (assembly_name.empty())
            return nullptr;

        auto it = _Assemblies.find(assembly_name);
        if (it == _Assemblies.end())
            return nullptr;

        auto load_assembly_function = GetLoadAssemblyFunction(assembly_name, it->second.AssemblyPath, it->second.AssemblyConfig, hostfxr_delegate_type::hdt_load_assembly_and_get_function_pointer);
        if (load_assembly_function == nullptr)
            return nullptr;

        void* entryPoint = nullptr;

        load_assembly_function(
            UTF8_TO_HOSTFXR(it->second.AssemblyPath).c_str(),
            UTF8_TO_HOSTFXR(full_qualified_class_name).c_str(),
            UTF8_TO_HOSTFXR(static_public_function_name).c_str(),
            full_qualifed_delegate_type.empty() ? nullptr : UTF8_TO_HOSTFXR(full_qualifed_delegate_type).c_str(),
            nullptr,
            &entryPoint);

        return entryPoint;
    }
};

DotNetCoreHost::DotNetCoreHost()
{
    _Impl = new DotNetCoreHostImpl();
}

DotNetCoreHost::~DotNetCoreHost()
{
    delete _Impl; _Impl = nullptr;
}

bool DotNetCoreHost::LoadDotNetCoreHost(std::string const& dotnet_root, std::string const& dotnet_version)
{
    return _Impl->LoadDotNetCoreHost(dotnet_root, dotnet_version);
}

DefaultDelegateType DotNetCoreHost::LoadAssemblyAndEntryPoint(std::string const& assembly_entry_point_class, std::string const& assembly_entry_point_function, std::string const& assembly_path, std::string const& assembly_runtimeconfig)
{
    return _Impl->LoadAssemblyAndEntryPoint(assembly_entry_point_class, assembly_entry_point_function, assembly_path, assembly_runtimeconfig);
}

DefaultDelegateType DotNetCoreHost::GetFunctionDelegate(std::string const& full_qualified_class_name, std::string const& static_public_function_name)
{
    return reinterpret_cast<DefaultDelegateType>(_Impl->GetFunctionDelegate(full_qualified_class_name, static_public_function_name, std::string()));
}

void* DotNetCoreHost::GetFunctionDelegate(std::string const& full_qualified_class_name, std::string const& static_public_function_name, std::string const& full_qualifed_delegate_type)
{
    return _Impl->GetFunctionDelegate(full_qualified_class_name, static_public_function_name, full_qualifed_delegate_type);
}

}
}