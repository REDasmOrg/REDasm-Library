#pragma once

#include <cstdint>
#include "../../pimpl.h"

namespace REDasm {

enum class ArgumentType { Null = 0, Integer, Float, String, Pointer };

struct Argument
{
    Argument(int64_t v);
    Argument(uint64_t v);
    Argument(double v);
    Argument(const char* v);
    Argument(void* v);

    ArgumentType type;

    union {
        uint64_t i_value;
        double d_value;
        const char* s_value;
        void* p_value;
    };

    template<typename T> T* pointer() const { return reinterpret_cast<T*>(p_value); }
    template<typename T> T integer() const { return static_cast<T>(i_value); }
};

class ArgumentListImpl;

class ArgumentList
{
    PIMPL_DECLARE_P(ArgumentList)
    PIMPL_DECLARE_PRIVATE(ArgumentList)

    public:
        ArgumentList();
        ArgumentList(const std::initializer_list<Argument>& args);
        size_t size() const;
        bool expect(const std::initializer_list<ArgumentType>& args) const;
        ArgumentList& operator<<(uint64_t rhs);
        ArgumentList& operator<<(double rhs);
        ArgumentList& operator<<(const char* rhs);
        ArgumentList& operator<<(void* rhs);
        const Argument& first() const;
        const Argument& last() const;
        const Argument& operator[](size_t idx) const;
};

} // namespace REDasm
