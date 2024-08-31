#pragma once

#include <cstdint>
#include <type_traits>
#include <stdexcept>
#include <string>
#include <vector>

namespace RegistryManipulator
{
    namespace Roots
    {
        // HKEY_CLASSES_ROOT
        static constexpr uint32_t classes_root = ((uint32_t)0x80000000);
        // HKEY_CURRENT_USER
        static constexpr uint32_t current_user = ((uint32_t)0x80000001);
        // HKEY_LOCAL_MACHINE
        static constexpr uint32_t local_machine = ((uint32_t)0x80000002);
        // HKEY_USERS
        static constexpr uint32_t users = ((uint32_t)0x80000003);
    }

    namespace Rights
    {
        static constexpr uint32_t read_control = 0x00020000u;
        static constexpr uint32_t synchronize = 0x00100000u;

        static constexpr uint32_t standard_rights_read = read_control;
        static constexpr uint32_t standard_rights_write = read_control;
        static constexpr uint32_t standard_rights_execute = read_control;
        static constexpr uint32_t standard_rights_all = 0x001F0000u;

        static constexpr uint32_t maximum_allowed = 0x02000000u;

        static constexpr uint32_t query_value = 0x0001;
        static constexpr uint32_t set_value = 0x0002;
        static constexpr uint32_t create_sub_key = 0x0004;
        static constexpr uint32_t enumerate_sub_keys = 0x0008;
        static constexpr uint32_t notify = 0x0010;
        static constexpr uint32_t create_link = 0x0020;
        static constexpr uint32_t wow64_32key = 0x0200;
        static constexpr uint32_t wow64_64key = 0x0100;
        static constexpr uint32_t wow64_res = 0x0300;

        static constexpr uint32_t read = (standard_rights_read | query_value | enumerate_sub_keys | notify) & ~synchronize;
        static constexpr uint32_t write = (standard_rights_write | set_value | create_sub_key) & ~synchronize;
        static constexpr uint32_t read_write = read | write;
        static constexpr uint32_t excute = read;
        static constexpr uint32_t all_access = (standard_rights_all | query_value | set_value | create_sub_key | enumerate_sub_keys | notify | create_link) & ~synchronize;
    }

    class Value
    {
        friend class Key;
    public:
        enum class Type
        {
            None,
            StringNul,
            StringExpandNul,
            Binary,
            Dword,
            DwordLittleEndian = Dword,
            DwordBigEndian,
            Link,
            MultipleStrings,
            ResourceList,
            FullResourceDescriptor,
            ResourceRequirementsList,
            Qword,
            QwordLittleEndian = Qword,
        };

    private:
        std::vector<uint8_t> _Buffer;
        Type _Type = Type::None;

    public:
        inline Type GetType() const { return _Type; }

        inline bool IsString() const { return _Type == Type::StringNul || _Type == Type::StringExpandNul; }

        inline size_t GetSize() const { return _Buffer.size(); }

        inline std::wstring GetString() const
        {
            if (_Type != Type::StringNul && _Type != Type::StringExpandNul)
                throw std::logic_error("Triied to get a string item while the value is not string.");

            return std::wstring((wchar_t*)_Buffer.data(), (wchar_t*)(_Buffer.data() + _Buffer.size() - 1));
        }

        inline std::vector<std::wstring> GetMultiString() const
        {
            if (_Type != Type::MultipleStrings)
                throw std::logic_error("Triied to get a string array while the value is not string array.");

            std::vector<std::wstring> res;

            const wchar_t* pBegin = (const wchar_t*)_Buffer.data();
            const wchar_t* pEnd = (const wchar_t*)(_Buffer.data() + _Buffer.size());
            const wchar_t* pItem = pBegin;
            while (pBegin != pEnd)
            {
                if (*pBegin == L'\0' && pBegin != pItem)
                {
                    res.emplace_back(pItem, pBegin);
                    pItem = ++pBegin;
                }

                ++pBegin;
            }

            return res;
        }

        template<typename T,
            typename std::enable_if<!std::is_pointer<T>::value, bool>::type = true,
            typename std::enable_if<!std::is_reference<T>::value, bool>::type = true,
            typename std::enable_if<std::is_scalar<T>::value || (std::is_class<T>::value && !std::is_polymorphic<T>::value), bool>::type = true>
        inline const T& GetBinary() const
        {
            if (_Type != Type::Binary)
                throw std::logic_error("Triied to get a binary item while the value is not binary.");
            if (_Buffer.size() != sizeof(T))
                throw std::length_error("Triied to cast a binary item when the size didn't match.");
            return *reinterpret_cast<const T*>(_Buffer.data());
        }

        inline const void* GetRawBinary() const
        {
            if (_Type != Type::Binary)
                throw std::logic_error("Triied to get a binary item while the value is not binary.");
            return reinterpret_cast<const void*>(_Buffer.data());
        }

        inline uint32_t GetDword() const
        {
            if (_Type != Type::Dword)
                throw std::logic_error("Triied to get a dword item while the value is not dword.");
            return *reinterpret_cast<const uint32_t*>(_Buffer.data());
        }

        inline uint64_t GetQword() const
        {
            if (_Type != Type::Qword)
                throw std::logic_error("Triied to get a qword item while the value is not qword.");
            return *reinterpret_cast<const uint64_t*>(_Buffer.data());
        }
    };

    class Key
    {
        void* _HKey;

        int _OpenKey(void* hRoot, const wchar_t* szSubKey, uint32_t rights, bool create);

        int _OpenKey(const wchar_t* szKey, uint32_t rights, bool create);

        ::RegistryManipulator::Key _OpenSubKey(const wchar_t* szSubKey, uint32_t rights, bool create);

        ::RegistryManipulator::Value _ReadValue(const wchar_t* szKey);

    public:
        Key();
        Key(Key const& other);
        Key(Key&& other) noexcept;

        Key& operator=(::RegistryManipulator::Key const& other);
        Key& operator=(::RegistryManipulator::Key&& other) noexcept;

        ~Key();

        static Key OpenRootKey(void* hRoot, const wchar_t* szSubKey, uint32_t rights, bool create);

        static Key OpenRootKey(void* hRoot, std::wstring const& szSubKey, uint32_t rights, bool create);

        static Key OpenRootKey(void* hRoot, const char* szSubKey, uint32_t rights, bool create);

        static Key OpenRootKey(void* hRoot, std::string const& szSubKey, uint32_t rights, bool create);

        // Convenient function that will deduce hRoot from the szKey prefix: HKEY_LOCAL_MACHINE\\SOME\\SUB\\KEY, HKEY_LOCAL_MACHINE will be used as hRoot
        static Key OpenRootKey(const wchar_t* szKey, uint32_t rights, bool create);

        static Key OpenRootKey(std::wstring const& szKey, uint32_t rights, bool create);

        static Key OpenRootKey(const char* szKey, uint32_t rights, bool create);

        static Key OpenRootKey(std::string const& szKey, uint32_t rights, bool create);

        void CloseKey();

        bool IsOpen() const;

        inline operator bool() const { return IsOpen(); }

        ::RegistryManipulator::Key OpenSubKey(const wchar_t* szSubKey, uint32_t rights, bool create);

        ::RegistryManipulator::Key OpenSubKey(std::wstring const& szSubKey, uint32_t rights, bool create);

        ::RegistryManipulator::Key OpenSubKey(const char* szSubKey, uint32_t rights, bool create);

        ::RegistryManipulator::Key OpenSubKey(std::string const& szSubKey, uint32_t rights, bool create);

        ::RegistryManipulator::Value ReadValue(const wchar_t* szKey);

        ::RegistryManipulator::Value ReadValue(std::wstring const& szKey);

        ::RegistryManipulator::Value ReadValue(const char* szKey);

        ::RegistryManipulator::Value ReadValue(std::string const& szKey);
    };
}