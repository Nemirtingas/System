#include "WinRegister.h"
#include <System/Encoding.hpp>

#include <windows.h>

namespace RegistryManipulator
{
    int Key::_OpenKey(void* hRoot, const wchar_t* szSubKey, uint32_t rights, bool create)
    {
        if (hRoot == nullptr || szSubKey == nullptr)
            return -1;

        CloseKey();

        LSTATUS status;
        DWORD dwDisposition;

        status = RegOpenKeyExW((HKEY)hRoot, szSubKey, 0, rights, (PHKEY)&_HKey);
        if (status == ERROR_FILE_NOT_FOUND && create)
        {
            status = RegCreateKeyExW((HKEY)hRoot, szSubKey, 0, 0, 0, rights, 0, (PHKEY)&_HKey, &dwDisposition);
        }

        if (status != ERROR_SUCCESS)
            return -1;

        return 0;
    }

    int Key::_OpenKey(const wchar_t* szKey, uint32_t rights, bool create)
    {
        if (szKey == nullptr)
            return -1;

        void* hRoot = nullptr;
        const wchar_t* szSubKey;

        if (wcsstr(szKey, L"HKEY_LOCAL_MACHINE") == szKey)
        {
            hRoot = (void*)Roots::local_machine;
        }
        else if (wcsstr(szKey, L"HKEY_CURRENT_USER") == szKey)
        {
            hRoot = (void*)Roots::current_user;
        }
        else if (wcsstr(szKey, L"HKEY_CLASSES_ROOT") == szKey)
        {
            hRoot = (void*)Roots::classes_root;
        }
        else if (wcsstr(szKey, L"HKEY_USERS") == szKey)
        {
            hRoot = (void*)Roots::users;
        }
        else
        {
            return -1;
        }

        szSubKey = wcsstr(szKey, L"\\");
        if (szSubKey == nullptr)
            return -1;

        return _OpenKey(hRoot, szSubKey + 1, rights, create);
    }

    Key Key::_OpenSubKey(const wchar_t* szSubKey, uint32_t rights, bool create)
    {
        Key sub_key;

        if (IsOpen())
        {
            LSTATUS status;
            DWORD dwDisposition;

            status = RegOpenKeyExW((HKEY)_HKey, szSubKey, 0, rights, (PHKEY)&sub_key._HKey);
            if (status == ERROR_FILE_NOT_FOUND && create)
            {
                status = RegCreateKeyExW((HKEY)_HKey, szSubKey, 0, 0, 0, rights, 0, (PHKEY)&sub_key._HKey, &dwDisposition);
            }
        }

        return sub_key;
    }

    Value Key::_ReadValue(const wchar_t* szKey)
    {
        Value value;

        if (IsOpen())
        {
            LSTATUS status;
            DWORD dwType = 0, dwType2 = 0;
            DWORD cbSize = 0;

            status = RegQueryValueExW((HKEY)_HKey, szKey, nullptr, &dwType, nullptr, &cbSize);
            if (status == ERROR_SUCCESS)
            {
                value._Buffer.resize(cbSize);
                status = RegQueryValueExW((HKEY)_HKey, szKey, nullptr, &dwType2, (LPBYTE)&value._Buffer[0], &cbSize);
                if (status == ERROR_SUCCESS && dwType == dwType2)
                {
                    value._Type = (Value::Type)dwType;
                }
            }
        }

        return value;
    }

    Key::Key() :
        _HKey(nullptr)
    {}

    Key::Key(Key const& other) :
        _HKey(nullptr)
    {
        if (other.IsOpen())
        {
            HANDLE hProcess = GetCurrentProcess();
            DuplicateHandle(hProcess, other._HKey, hProcess, &_HKey, 0, FALSE, DUPLICATE_SAME_ACCESS);
        }
    }

    Key::Key(Key&& other) noexcept :
        _HKey(other._HKey)
    {
        other._HKey = nullptr;
    }

    Key& Key::operator=(Key const& other)
    {
        Key tmp(other);
        *this = std::move(tmp);

        return *this;
    }

    Key& Key::operator=(Key&& other) noexcept
    {
        void* tmp = other._HKey;
        other._HKey = _HKey;
        _HKey = tmp;
        return *this;
    }

    Key::~Key()
    {
        CloseKey();
    }

    Key Key::OpenRootKey(void* hRoot, const wchar_t* szSubKey, uint32_t rights, bool create)
    {
        Key k;
        k._OpenKey(hRoot, szSubKey, rights, create);
        return k;
    }

    Key Key::OpenRootKey(void* hRoot, std::wstring const& szSubKey, uint32_t rights, bool create)
    {
        Key k;
        k._OpenKey(hRoot, szSubKey.c_str(), rights, create);
        return k;
    }

    Key Key::OpenRootKey(void* hRoot, const char* szSubKey, uint32_t rights, bool create)
    {
        Key k;
        std::wstring wstr = System::Encoding::Utf8ToWChar(szSubKey);        
        if (!wstr.empty())
            k._OpenKey(hRoot, wstr.c_str(), rights, create);

        return k;
    }

    Key Key::OpenRootKey(void* hRoot, std::string const& szSubKey, uint32_t rights, bool create)
    {
        return OpenRootKey(hRoot, szSubKey.c_str(), rights, create);
    }

    Key Key::OpenRootKey(const wchar_t* szKey, uint32_t rights, bool create)
    {
        Key k;
        k._OpenKey(szKey, rights, create);
        return k;
    }

    Key Key::OpenRootKey(std::wstring const& szKey, uint32_t rights, bool create)
    {
        Key k;
        k._OpenKey(szKey.c_str(), rights, create);
        return k;
    }

    Key Key::OpenRootKey(const char* szKey, uint32_t rights, bool create)
    {
        Key k;
        std::wstring wstr = System::Encoding::Utf8ToWChar(szKey);
        if (!wstr.empty())
            k._OpenKey(wstr.c_str(), rights, create);

        return k;
    }

    Key Key::OpenRootKey(std::string const& szKey, uint32_t rights, bool create)
    {
        return OpenRootKey(szKey.c_str(), rights, create);
    }

    void Key::CloseKey()
    {
        if (_HKey != nullptr)
        {
            RegCloseKey((HKEY)_HKey);
            _HKey = nullptr;
        }
    }

    bool Key::IsOpen() const
    {
        return _HKey != nullptr;
    }

    Key Key::OpenSubKey(const wchar_t* szSubKey, uint32_t rights, bool create)
    {
        return _OpenSubKey(szSubKey, rights, create);
    }

    Key Key::OpenSubKey(std::wstring const& szSubKey, uint32_t rights, bool create)
    {
        return _OpenSubKey(szSubKey.c_str(), rights, create);
    }

    Key Key::OpenSubKey(const char* szSubKey, uint32_t rights, bool create)
    {
        std::wstring wstr = System::Encoding::Utf8ToWChar(szSubKey);
        if (!wstr.empty())
            return _OpenSubKey(wstr.c_str(), rights, create);

        return Key();
    }

    Key Key::OpenSubKey(std::string const& szSubKey, uint32_t rights, bool create)
    {
        return OpenSubKey(szSubKey.c_str(), rights, create);
    }

    Value Key::ReadValue(const wchar_t* szKey)
    {
        return _ReadValue(szKey);
    }

    Value Key::ReadValue(std::wstring const& szKey)
    {
        return _ReadValue(szKey.c_str());
    }

    Value Key::ReadValue(const char* szKey)
    {
        std::wstring wstr = System::Encoding::Utf8ToWChar(szKey);
        if (!wstr.empty())
            return _ReadValue(wstr.c_str());

        return Value();
    }

    Value Key::ReadValue(std::string const& szKey)
    {
        return ReadValue(szKey.c_str());
    }

}