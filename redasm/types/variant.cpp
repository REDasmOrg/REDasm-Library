#include "variant.h"
#include <impl/types/variant_impl.h>
#include <impl/libs/cereal/cereal.hpp>
#include <impl/libs/cereal/archives/binary.hpp>

namespace REDasm {

Variant::Variant(): m_pimpl_p(new VariantImpl()) {  }
Variant::Variant(s8 v): m_pimpl_p(new VariantImpl(v)) {  }
Variant::Variant(s16 v): m_pimpl_p(new VariantImpl(v)) {  }
Variant::Variant(s32 v): m_pimpl_p(new VariantImpl(v)) {  }
Variant::Variant(s64 v): m_pimpl_p(new VariantImpl(v)) {  }
Variant::Variant(u8 v): m_pimpl_p(new VariantImpl(v)) {  }
Variant::Variant(u16 v): m_pimpl_p(new VariantImpl(v)) {  }
Variant::Variant(u32 v): m_pimpl_p(new VariantImpl(v)) {  }
Variant::Variant(u64 v): m_pimpl_p(new VariantImpl(v)) {  }
Variant::Variant(const char *v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Variant(const String &v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Variant(Object *v): m_pimpl_p(new VariantImpl(v)) {  }
Variant::Type Variant::type() const { PIMPL_P(const Variant); return p->type(); }
bool Variant::isNull() const { PIMPL_P(const Variant); return p->isNull(); }
bool Variant::isInteger() const { PIMPL_P(const Variant); return p->isNull(); }
bool Variant::isString() const { PIMPL_P(const Variant); return p->isString(); }
bool Variant::isObject() const { PIMPL_P(const Variant); return p->isObject(); }
bool Variant::objectIs(object_id_t id) const { PIMPL_P(const Variant); return p->objectIs(id); }
s8 Variant::toS8()   const { PIMPL_P(const Variant); return p->toS8();  }
s16 Variant::toS16() const { PIMPL_P(const Variant); return p->toS16(); }
s32 Variant::toS32() const { PIMPL_P(const Variant); return p->toS32(); }
s64 Variant::toS64() const { PIMPL_P(const Variant); return p->toS32(); }
u16 Variant::toU8()  const { PIMPL_P(const Variant); return p->toS8();  }
u16 Variant::toU16() const { PIMPL_P(const Variant); return p->toS16(); }
u32 Variant::toU32() const { PIMPL_P(const Variant); return p->toS32(); }
u64 Variant::toU64() const { PIMPL_P(const Variant); return p->toS64(); }
Object *Variant::toObject() const { PIMPL_P(const Variant); return p->toObject(); }
String Variant::toString()  const { PIMPL_P(const Variant); return p->toString(); }
Variant &Variant::operator=(s8 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(s16 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(s32 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(s64 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(u8 v)  { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(u16 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(u32 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(u64 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(const char *v)   { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(const String &v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(Object *v)       { PIMPL_P(Variant); p->set(v); return *this; }
bool Variant::operator==(const Variant &rhs) const { PIMPL_P(const Variant); return p->equals(rhs);  }
bool Variant::operator!=(const Variant &rhs) const { PIMPL_P(const Variant); return !p->equals(rhs); }

template<typename Archive> void Variant::save(Archive &a) const
{
    PIMPL_P(const Variant);
    a(p->m_keeper.type);

    switch(p->m_keeper.type)
    {
        case Type::S8:  a(p->m_keeper.value.s8_); break;
        case Type::S16: a(p->m_keeper.value.s16_); break;
        case Type::S32: a(p->m_keeper.value.s32_); break;
        case Type::S64: a(p->m_keeper.value.s64_); break;
        case Type::U8:  a(p->m_keeper.value.u8_); break;
        case Type::U16: a(p->m_keeper.value.u16_); break;
        case Type::U32: a(p->m_keeper.value.u32_); break;
        case Type::U64: a(p->m_keeper.value.u64_); break;
        case Type::String: a(*p->m_keeper.value.string); break;
        case Type::Object: p->m_keeper.value.object->save(a); break;
        default: break;
    }
}

template<typename Archive> void Variant::load(Archive &a)
{
    PIMPL_P(Variant);
    a(p->m_keeper.type);

    switch(p->m_keeper.type)
    {
        case Type::S8:  a(p->m_keeper.value.s8_); break;
        case Type::S16: a(p->m_keeper.value.s16_); break;
        case Type::S32: a(p->m_keeper.value.s32_); break;
        case Type::S64: a(p->m_keeper.value.s64_); break;
        case Type::U8:  a(p->m_keeper.value.u8_); break;
        case Type::U16: a(p->m_keeper.value.u16_); break;
        case Type::U32: a(p->m_keeper.value.u32_); break;
        case Type::U64: a(p->m_keeper.value.u64_); break;

        case Type::String:
            p->m_keeper.value.string = new String();
            a(*p->m_keeper.value.string);
            break;

        case Type::Object: p->m_keeper.value.object->load(a); break;
        default: break;
    }
}

template void Variant::save<cereal::BinaryOutputArchive>(cereal::BinaryOutputArchive&) const;
template void Variant::load<cereal::BinaryInputArchive>(cereal::BinaryInputArchive&);

} // namespace REDasm
