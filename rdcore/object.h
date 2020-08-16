#pragma once

#define CPTR(type, ptr) reinterpret_cast<type*>(ptr)
#define CPTR_P(type, ptr) type* p = CPTR(type, ptr)

class Object
{
    public:
        Object() = default;
        virtual ~Object() = default;
};
