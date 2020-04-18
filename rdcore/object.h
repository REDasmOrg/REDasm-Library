#pragma once

#define CPTR(type, ptr) reinterpret_cast<type*>(ptr)
#define CPTR_P(type, ptr) type* p = C_PTR(type, ptr)

class Object
{
    public:
        Object() = default;
        virtual ~Object() = default;
};
