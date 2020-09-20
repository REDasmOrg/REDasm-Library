#pragma once

#define CPTR(type, ptr) reinterpret_cast<type*>(ptr)
#define CPTR_P(type, ptr) type* p = CPTR(type, ptr)

class EventDispatcher;

class Object
{
    public:
        Object() = default;
        Object(EventDispatcher* dispatcher);
        virtual ~Object() = default;

    public:
        EventDispatcher* dispatcher() const;

    private:
        EventDispatcher* m_dispatcher{nullptr};
};
