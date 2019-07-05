#pragma once

#include <redasm/types/variant.h>

namespace REDasm {

class VariantImpl
{
    PIMPL_DECLARE_Q(Variant)
    PIMPL_DECLARE_PUBLIC(Variant)

    private:
        struct TypeKeeper {
            TypeKeeper(): type(Variant::INVALID) { value.object = nullptr; }

            union {
                s8 s8_; s16 s16_; s32 s32_; s64 s64_;
                u8 u8_; u16 u16_; u32 u32_; u64 u64_;
                void* pointer;
                String* string;
                Object* object;
            } value;

            Variant::Type type;
        };

    public:
        VariantImpl();
        VariantImpl(s8 v);
        VariantImpl(s16 v);
        VariantImpl(s32 v);
        VariantImpl(s64 v);
        VariantImpl(u8 v);
        VariantImpl(u16 v);
        VariantImpl(u32 v);
        VariantImpl(u64 v);
        VariantImpl(void* v);
        VariantImpl(const char* v);
        VariantImpl(const String& v);
        VariantImpl(Object* v);
        ~VariantImpl();

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
        void set(s8 v);
        void set(s16 v);
        void set(s32 v);
        void set(s64 v);
        void set(u8 v);
        void set(u16 v);
        void set(u32 v);
        void set(u64 v);
        void set(void *v);
        void set(const String& v);
        void set(Object* v);
        bool equals(const Variant& rhs) const;

    private:
        void deallocateString();

    private:
        TypeKeeper m_keeper;
};

} // namespace REDasm
