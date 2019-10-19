#pragma once

#include <memory>

#ifdef LIBREDASM_STATIC_DEFINE
#  define LIBREDASM_API
#  define LIBREDASM_NO_API
#else
#  ifndef LIBREDASM_API
#    ifdef LibREDasm_APIS
        /* We are building this library */
#      define LIBREDASM_API __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define LIBREDASM_API __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef LIBREDASM_NO_API
#    define LIBREDASM_NO_API __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef LIBREDASM_DEPRECATED
#  define LIBREDASM_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef LIBREDASM_DEPRECATED_API
#  define LIBREDASM_DEPRECATED_API LIBREDASM_API LIBREDASM_DEPRECATED
#endif

#ifndef LIBREDASM_DEPRECATED_NO_API
#  define LIBREDASM_DEPRECATED_NO_API LIBREDASM_NO_API LIBREDASM_DEPRECATED
#endif

#define EF_OP_IMPL_UNARY(Enum, op, ret) constexpr ret operator op(Enum lhs) { \
                                      return static_cast<ret>(op static_cast<std::underlying_type<Enum>::type>(lhs)); \
                                  }

#define EF_OP_IMPL(Enum, op, ret) constexpr ret operator op(Enum lhs, Enum rhs) { \
                                      return static_cast<ret>(static_cast<std::underlying_type<Enum>::type>(lhs) op \
                                                              static_cast<std::underlying_type<Enum>::type>(rhs)); \
                                  }

#define EF_OP_ASSIGN_IMPL(Enum, op) inline Enum& operator op##=(Enum& lhs, Enum rhs) { \
                                        lhs = static_cast<Enum>(static_cast<std::underlying_type<Enum>::type>(lhs) op \
                                                                static_cast<std::underlying_type<Enum>::type>(rhs)); \
                                        return lhs; \
                                    }

#define ENUM_FLAGS_OPERATORS(Enum) EF_OP_IMPL(Enum, |, Enum)  \
                                   EF_OP_IMPL(Enum, &, size_t)  \
                                   EF_OP_ASSIGN_IMPL(Enum, |) \
                                   EF_OP_ASSIGN_IMPL(Enum, &) \
                                   EF_OP_IMPL_UNARY(Enum, ~, Enum)


#define STRINGIFY(x)        #x
#define DO_UNPAREN(...)     __VA_ARGS__
#define INVOKE(expr)        expr
#define UNPAREN(args)       INVOKE(DO_UNPAREN args)

#define ENTRY_FUNCTION                             "__redasm_entry__"
#define START_FUNCTION                             "__redasm_start__"
#define REGISTER_INVALID                           REDasm::npos
#define BRANCH_DIRECTION(instruction, destination) (static_cast<int>(destination) - static_cast<int>(instruction->address))

#define DECLARE_DEFAULT_DELETER(type) \
    namespace std { \
        template<> class LIBREDASM_API default_delete<type> { \
            public: void operator()(type* t); \
        }; \
    }

#define DEFINE_DEFAULT_DELETER(type) \
    namespace std { \
        void std::default_delete<type>::operator()(type* t) { delete t; } \
    }

#if __cplusplus <= 201103L && __GNUC__
namespace std {
template<typename T, typename... Args> std::unique_ptr<T> make_unique(Args&&... args) { return std::unique_ptr<T>(new T(std::forward<Args>(args)...)); }
} // namespace std
#endif
