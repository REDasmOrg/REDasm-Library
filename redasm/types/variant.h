#pragma once

#include "base.h"
#include "../pimpl.h"

namespace REDasm {

class VariantImpl;
class Object;
class String;

class Variant
{
    PIMPL_DECLARE_P(Variant)
    PIMPL_DECLARE_PRIVATE(Variant)

    public:
        enum Type: argument_t {
            INVALID = 0,
            S8, S16, S32, S64,
            U8, U16, U32, U64,
            POINTER, STRING, OBJECT,
            INTEGER = argument_t(-1) // Internal use
        };

    public:
        Variant();
        Variant(s8 v);
        Variant(s16 v);
        Variant(s32 v);
        Variant(s64 v);
        Variant(u8 v);
        Variant(u16 v);
        Variant(u32 v);
        Variant(u64 v);
        Variant(void* v);
        Variant(const char* v);
        Variant(const String& v);
        Variant(Object* v);

    public:
        Variant::Type type() const;
        bool isInvalid() const;
        bool isInteger() const;
        bool isString() const;
        bool isObject() const;
        bool isPointer() const;
        bool objectIs(object_id_t id) const;
        s8 toS8() const;
        s16 toS16() const;
        s32 toS32() const;
        s64 toS64() const;
        u16 toU8() const;
        u16 toU16() const;
        u32 toU32() const;
        u64 toU64() const;
        void* toPointer() const;
        Object* toObject() const;
        String toString() const;

     public:
        Variant& operator=(s8 v);
        Variant& operator=(s16 v);
        Variant& operator=(s32 v);
        Variant& operator=(s64 v);
        Variant& operator=(u8 v);
        Variant& operator=(u16 v);
        Variant& operator=(u32 v);
        Variant& operator=(u64 v);
        Variant& operator=(void* v);
        Variant& operator=(const char* v);
        Variant& operator=(const String& v);
        Variant& operator=(Object* v);
        bool operator==(const Variant& rhs) const;
        bool operator!=(const Variant& rhs) const;

    public:
        template<typename Archive> void save(Archive& a) const;
        template<typename Archive> void load(Archive& a);
};

} // namespace REDasm

template<typename T> T* variant_object(const REDasm::Variant& v) { return reinterpret_cast<T*>(v.toObject()); }
template<typename T> T* variant_pointer(const REDasm::Variant& v) { return reinterpret_cast<T*>(v.toPointer()); }
