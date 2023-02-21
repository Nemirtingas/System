#pragma once 

#include <System/StringView.hpp>
#include <array>

namespace System {
namespace TypeName {

namespace detail {

template <std::size_t...Idxs>
constexpr auto substring_as_array(System::StringView str, std::index_sequence<Idxs...> idxs)
{
    return std::array<char, idxs.size()>{ str[Idxs]... };
}

template <typename T>
constexpr auto type_name_array()
{
#if defined(__clang__)
    constexpr auto prefix = System::StringView{ "[T = " };
    constexpr auto suffix = System::StringView{ "]" };
    constexpr auto function = System::StringView{ __PRETTY_FUNCTION__ };
#elif defined(__GNUC__)
    constexpr auto prefix = System::StringView{ "with T = " };
    constexpr auto suffix = System::StringView{ "]" };
    constexpr auto function = System::StringView{ __PRETTY_FUNCTION__ };
#elif defined(_MSC_VER)
    constexpr auto prefix = System::StringView{ "type_name_array<" };
    constexpr auto suffix = System::StringView{ ">(void)" };
    constexpr auto function = System::StringView{ __FUNCSIG__ };
#else
# error Unsupported compiler
#endif

    constexpr auto start = function.find(prefix) + prefix.size();
    constexpr auto end = function.rfind(suffix);

    static_assert(start < end, "start >= end");

#if defined(_MSC_VER)
    constexpr auto class_prefix = System::StringView{ "class " };
    constexpr auto struct_prefix = System::StringView{ "struct " };

    constexpr auto win_name = function.substr(start, (end - start));

    constexpr auto class_start = win_name.find(class_prefix) + class_prefix.size();
    constexpr auto struct_start = win_name.find(struct_prefix) + struct_prefix.size();

    constexpr auto name = class_start == class_prefix.size() ?
        win_name.substr(class_start) :
        struct_start == struct_prefix.size() ? win_name.substr(struct_start) : win_name;
#else
    constexpr auto name = function.substr(start, (end - start));
#endif

    return substring_as_array(name, std::make_index_sequence<name.size()>{});
}


#if defined(_MSC_VER)
#if !defined(__clang__)
    #define MSVC_NOT_INLINE_CXX14
#else
    #define MSVC_NOT_INLINE_CXX14 inline
#endif
#else
    #define MSVC_NOT_INLINE_CXX14 inline
#endif

template <typename T>
struct type_name_holder {
    static MSVC_NOT_INLINE_CXX14 constexpr auto value = type_name_array<T>();
};


template <typename T>
struct remove_all_ref_ptr { typedef T type; };

template <typename T>
struct remove_all_ref_ptr<T*> : public remove_all_ref_ptr<T> { };

template <typename T>
struct remove_all_ref_ptr<T* const> : public remove_all_ref_ptr<T> { };

template <typename T>
struct remove_all_ref_ptr<T* volatile> : public remove_all_ref_ptr<T> { };

template <typename T>
struct remove_all_ref_ptr<T* const volatile> : public remove_all_ref_ptr<T> { };

template <typename T>
struct remove_all_ref_ptr<T&> : public remove_all_ref_ptr<T> { };

template <typename T>
struct remove_all_ref_ptr<T&&> : public remove_all_ref_ptr<T> { };

template <typename T>
using remove_all_ref_ptr_t = typename remove_all_ref_ptr<T>::type;

}

template <typename T>
constexpr auto BaseTypeName() -> System::StringView
{
    constexpr auto& value = detail::type_name_holder<detail::remove_all_ref_ptr_t<T>>::value;
    return System::StringView{ value.data(), value.size() };
}

template <typename T>
constexpr auto BaseTypeName(T&&) -> System::StringView
{
    constexpr auto& value = detail::type_name_holder<detail::remove_all_ref_ptr_t<T>>::value;
    return System::StringView{ value.data(), value.size() };
}

template <typename T>
constexpr auto TypeName() -> System::StringView
{
    constexpr auto& value = detail::type_name_holder<T>::value;
    return System::StringView{ value.data(), value.size() };
}

template <typename T>
constexpr auto TypeName(T&&) -> System::StringView
{
    constexpr auto& value = detail::type_name_holder<T>::value;
    return System::StringView{ value.data(), value.size() };
}

}
}