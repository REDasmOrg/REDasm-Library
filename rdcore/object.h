#pragma once

#include <string>

#define CPTR(type, ptr) reinterpret_cast<type*>(ptr)
#define CPTR_P(type, ptr) type* p = CPTR(type, ptr)

class Context;

class Object
{
    public:
        Object() = default;
        Object(Context* context);
        virtual ~Object() = default;

    public:
        Context* context() const;
        void log(const std::string& s) const;
        void status(const std::string& s) const;

    private:
        Context* m_context{nullptr};
};
