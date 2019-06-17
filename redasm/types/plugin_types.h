#pragma once

#include <memory>
#include "../macros.h"
#include "../support/safe_ptr.h"

namespace REDasm {

class Object
{
    public:
        virtual void release();
};

template<typename T> struct ObjectDeleter
{
    void operator()(T* t) { t->release(); }
};

template<typename T> using object_ptr = std::unique_ptr< T, ObjectDeleter<T> >;
template<typename T> object_ptr<T> wrap_object(Object* obj) { return object_ptr<T>(static_cast<T*>(obj)); }
template<typename T> safe_ptr<T> wrap_safe_object(Object* obj) { return safe_ptr<T>(static_cast<T*>(obj), ObjectDeleter<T>()); }
template<typename T, typename... Args> object_ptr<T> make_object(Args&& ...args) { return wrap_object<T>(new T(std::forward<Args>(args)...)); }

} // namespace REDasm
