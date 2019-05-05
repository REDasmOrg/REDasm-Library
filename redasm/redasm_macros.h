#pragma once

#include <memory>

#define EF_OP_IMPL(Enum, op, ret) constexpr ret operator op(Enum lhs, Enum rhs) { \
                                      return static_cast<ret>(static_cast<std::underlying_type<Enum>::type>(lhs) op \
                                                              static_cast<std::underlying_type<Enum>::type>(rhs)); \
                                  }

#define EF_OP_ASSIGN_IMPL(Enum, op) inline Enum& operator op##=(Enum& lhs, Enum rhs) { \
                                        lhs = static_cast<Enum>(static_cast<std::underlying_type<Enum>::type>(lhs) op \
                                                                static_cast<std::underlying_type<Enum>::type>(rhs)); \
                                        return lhs; \
                                    }

#define ENUM_FLAGS_OPERATORS(Enum) EF_OP_IMPL(Enum, |, Enum) \
                                   EF_OP_IMPL(Enum, &, bool) \
                                   EF_OP_ASSIGN_IMPL(Enum, |)

#define DO_UNPAREN(...) __VA_ARGS__
#define INVOKE(expr)    expr
#define UNPAREN(args)   INVOKE(DO_UNPAREN args)

#define RE_UNUSED(x)   (void)x

#if __cplusplus <= 201103L && __GNUC__
namespace std {
template<typename T, typename... Args> std::unique_ptr<T> make_unique(Args&&... args) { return std::unique_ptr<T>(new T(std::forward<Args>(args)...)); }
}
#endif

