#pragma once

#include <type_traits>
#include "../base.h"
#include "../macros.h"

#define DEFAULT_NAME_SIZE      32
#define DEFAULT_CONTAINER_SIZE 8

#define FORCE_STANDARD_LAYOUT(T) static_assert(std::is_standard_layout<T>::value, "");

#define BASIC_STRUCT_COPYABLE(T) \
    T(const T& rhs) = default; \
    T(const T##Struct& rhs) { *this = rhs; } \
    T& operator=(const T& rhs) = default;

namespace REDasm {

typedef u32 type_t;
typedef u32 flag_t;

template<typename T, typename V> inline bool typeIs(const T* t, V v) { return t->type == v; }
template<typename T, typename V> inline bool hasFlag(const T* t, V v) { return t->flags & v; }
template<typename T> inline bool isInvalid(T t) { static_assert(std::is_integral<T>::value, ""); return t == REDasm::npos; }
template<typename T> inline bool isValid(T t) { static_assert(std::is_integral<T>::value, ""); return t != REDasm::npos; }

template<typename BasicStruct>
struct ApiWrap: public BasicStruct
{
    typedef BasicStruct Struct;
};

} // namespace REDasm
