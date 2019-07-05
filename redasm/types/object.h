#pragma once

#include <memory>
#include "base.h"
#include "factory.h"
#include "../static/crc32.h"
#include "../support/safe_ptr.h"

#define REDASM_OBJECT(name) public: \
                                static constexpr const char* NAME = #name; \
                                static constexpr object_id_t ID = static_crc32(#name); \
                                const char* objectName() const override { return #name; } \
                                object_id_t objectId() const override { return static_crc32(#name); } \
                            private:

#define REDASM_FACTORY_OBJECT(name) REDASM_OBJECT(name) \
                                    public: \
                                    static bool REGISTERED; \
                                    private: \

#define REGISTER_FACTORY_OBJECT(name) bool name::REGISTERED = Factory::registerAs(ID, []()-> Object* { return new name(); });

namespace cereal {

class BinaryOutputArchive;
class BinaryInputArchive;

}

namespace REDasm {

class Object
{
    public:
        Object() = default;
        virtual ~Object() = default;
        bool objectIs(object_id_t id) const;
        virtual const char* objectName() const = 0;
        virtual object_id_t objectId() const = 0;
        virtual void save(class cereal::BinaryOutputArchive& a) const;
        virtual void load(class cereal::BinaryInputArchive& a);
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
