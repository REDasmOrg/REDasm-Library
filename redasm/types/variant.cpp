#include "variant.h"
#include <functional>
#include <cassert>
#include <impl/types/variant_impl.h>
#include <impl/libs/cereal/cereal.hpp>
#include <impl/libs/cereal/archives/binary.hpp>
#include "object.h"
#include "string.h"
#include "factory.h"

namespace REDasm {

Variant::Variant(): m_pimpl_p(new VariantImpl()) { }
Variant::Variant(s8 v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Variant(s16 v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Variant(s32 v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Variant(s64 v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Variant(u8 v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Variant(u16 v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Variant(u32 v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Variant(u64 v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Variant(void *v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Variant(const char *v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Variant(const String &v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Variant(Object *v): m_pimpl_p(new VariantImpl(v)) { }
Variant::Type Variant::type() const { PIMPL_P(const Variant); return p->type(); }
bool Variant::isInvalid() const { PIMPL_P(const Variant); return p->isInvalid(); }
bool Variant::isInteger() const { PIMPL_P(const Variant); return p->isInteger(); }
bool Variant::isString() const { PIMPL_P(const Variant); return p->isString(); }
bool Variant::isObject() const { PIMPL_P(const Variant); return p->isObject(); }
bool Variant::isPointer() const { PIMPL_P(const Variant); return p->isPointer(); }
bool Variant::objectIs(object_id_t id) const { PIMPL_P(const Variant); return p->objectIs(id); }
s8 Variant::toS8()   const { PIMPL_P(const Variant); return p->toS8();  }
s16 Variant::toS16() const { PIMPL_P(const Variant); return p->toS16(); }
s32 Variant::toS32() const { PIMPL_P(const Variant); return p->toS32(); }
s64 Variant::toS64() const { PIMPL_P(const Variant); return p->toS32(); }
u16 Variant::toU8()  const { PIMPL_P(const Variant); return p->toU8();  }
u16 Variant::toU16() const { PIMPL_P(const Variant); return p->toU16(); }
u32 Variant::toU32() const { PIMPL_P(const Variant); return p->toU32(); }
u64 Variant::toU64() const { PIMPL_P(const Variant); return p->toU64(); }
int Variant::toInt() const { PIMPL_P(const Variant); return p->toInt(); }
unsigned int Variant::toUInt() const { PIMPL_P(const Variant); return p->toUInt(); }
void *Variant::toPointer() const  { PIMPL_P(const Variant); return p->toPointer(); }
Object *Variant::toObject() const { PIMPL_P(const Variant); return p->toObject();  }
String Variant::toString()  const { PIMPL_P(const Variant); return p->toString();  }

Object *Variant::checkObject(object_id_t id) const
{
    Object* obj = this->toObject();

    if(id != Object::ID)
        assert(obj && (obj->objectId() == id));

    return obj;
}

Variant &Variant::operator=(s8 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(s16 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(s32 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(s64 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(u8 v)  { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(u16 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(u32 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(u64 v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(void *v)         { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(const char *v)   { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(const String &v) { PIMPL_P(Variant); p->set(v); return *this; }
Variant &Variant::operator=(Object *v)       { PIMPL_P(Variant); p->set(v); return *this; }
bool Variant::operator==(const Variant &rhs) const { PIMPL_P(const Variant); return p->equals(rhs); }
bool Variant::operator!=(const Variant &rhs) const { PIMPL_P(const Variant); return !p->equals(rhs); }
bool Variant::operator<=(const Variant &rhs) const { PIMPL_P(const Variant); return p->lte(rhs); }
bool Variant::operator>=(const Variant &rhs) const {  PIMPL_P(const Variant); return p->gte(rhs); }
bool Variant::operator<(const Variant &rhs) const { PIMPL_P(const Variant); return p->lt(rhs); }
bool Variant::operator>(const Variant &rhs) const { PIMPL_P(const Variant); return p->gt(rhs); }

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
        case Type::STRING: a(*p->m_keeper.value.string); break;

        case Type::OBJECT:
            a(p->m_keeper.value.object->objectId());
            p->m_keeper.value.object->save(a);
            break;

        default: assert(false);
    }
}

template<typename Archive> void Variant::load(Archive &a)
{
    PIMPL_P(Variant);
    a(p->m_keeper.type);

    object_id_t objid = 0;

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

        case Type::STRING:
            p->m_keeper.value.string = new String();
            a(*p->m_keeper.value.string);
            break;

        case Type::OBJECT:
            a(objid);
            p->m_keeper.value.object = Factory::create(objid);
            assert(p->m_keeper.value.object);
            p->m_keeper.value.object->load(a);
            break;

        default: assert(false);
    }
}

template void Variant::save<cereal::BinaryOutputArchive>(cereal::BinaryOutputArchive&) const;
template void Variant::load<cereal::BinaryInputArchive>(cereal::BinaryInputArchive&);

} // namespace REDasm

namespace std {

size_t hash<REDasm::Variant>::operator()(const REDasm::Variant &v) const noexcept
{
    switch(v.type())
    {
        case REDasm::Variant::Type::S8:  return std::hash<s8>()(v.toS8());
        case REDasm::Variant::Type::S16: return std::hash<s16>()(v.toS16());
        case REDasm::Variant::Type::S32: return std::hash<s32>()(v.toS32()); break;
        case REDasm::Variant::Type::S64: return std::hash<s64>()(v.toS64()); break;
        case REDasm::Variant::Type::U8:  return std::hash<u8>()(v.toU8()); break;
        case REDasm::Variant::Type::U16: return std::hash<u16>()(v.toU16()); break;
        case REDasm::Variant::Type::U32: return std::hash<u32>()(v.toU32()); break;
        case REDasm::Variant::Type::U64: return std::hash<u64>()(v.toU64()); break;
        case REDasm::Variant::Type::STRING: return std::hash<REDasm::String>()(v.toString());
        case REDasm::Variant::Type::OBJECT: return std::hash<REDasm::Object*>()(v.toObject());
        default: break;
    }

    assert(false);
    return 0;
}

size_t std::equal_to<REDasm::Variant>::operator()(const REDasm::Variant &v1, const REDasm::Variant &v2) const noexcept { return v1 == v2; }

} // namespace std
