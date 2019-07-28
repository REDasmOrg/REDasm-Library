#include "variant_impl.h"
#include <redasm/types/object.h>
#include <redasm/types/string.h>

namespace REDasm {

VariantImpl::VariantImpl()
{
    m_keeper.type = Variant::Type::INVALID;
    m_keeper.value.object = nullptr;
}

VariantImpl::VariantImpl(s8 v)  { this->set(v); }
VariantImpl::VariantImpl(s16 v) { this->set(v); }
VariantImpl::VariantImpl(s32 v) { this->set(v); }
VariantImpl::VariantImpl(s64 v) { this->set(v); }
VariantImpl::VariantImpl(u8 v)  { this->set(v); }
VariantImpl::VariantImpl(u16 v) { this->set(v); }
VariantImpl::VariantImpl(u32 v) { this->set(v); }
VariantImpl::VariantImpl(u64 v) { this->set(v); }
VariantImpl::VariantImpl(void *v)   { this->set(v); }
VariantImpl::VariantImpl(const char *v)   { this->set(v); }
VariantImpl::VariantImpl(const String &v) { this->set(v); }
VariantImpl::VariantImpl(Object *v)       { this->set(v); }
VariantImpl::~VariantImpl() { this->deallocateString(); }
Variant::Type VariantImpl::type() const { return m_keeper.type; }
bool VariantImpl::isInvalid() const { return m_keeper.type == Variant::Type::INVALID; }
bool VariantImpl::isInteger() const { return m_keeper.type >= Variant::Type::S8 && m_keeper.type <= Variant::Type::U64; }
bool VariantImpl::isString() const { return m_keeper.type == Variant::Type::STRING; }
bool VariantImpl::isObject() const { return m_keeper.type == Variant::Type::OBJECT; }
bool VariantImpl::isPointer() const { return m_keeper.type == Variant::Type::POINTER; }

bool VariantImpl::objectIs(object_id_t id) const
{
    if(m_keeper.type != Variant::Type::OBJECT)
        return false;

    return m_keeper.value.object->objectId() == id;
}

s8 VariantImpl::toS8()   const { return this->isInteger() ? m_keeper.value.s8_  :  s8(); }
s16 VariantImpl::toS16() const { return this->isInteger() ? m_keeper.value.s16_ : s16(); }
s32 VariantImpl::toS32() const { return this->isInteger() ? m_keeper.value.s32_ : s32(); }
s64 VariantImpl::toS64() const { return this->isInteger() ? m_keeper.value.s64_ : s64(); }
u16 VariantImpl::toU8()  const { return this->isInteger() ? m_keeper.value.u8_  :  u8(); }
u16 VariantImpl::toU16() const { return this->isInteger() ? m_keeper.value.u16_ : u16(); }
u32 VariantImpl::toU32() const { return this->isInteger() ? m_keeper.value.u32_ : u32(); }
u64 VariantImpl::toU64() const { return this->isInteger() ? m_keeper.value.u64_ : u64(); }

int VariantImpl::toInt() const
{
    if(!this->isInteger())
        return 0;

    if(std::is_same<int, decltype(m_keeper.value.s8_)>::value) return static_cast<int>(m_keeper.value.s8_);
    if(std::is_same<int, decltype(m_keeper.value.s16_)>::value) return static_cast<int>(m_keeper.value.s16_);
    if(std::is_same<int, decltype(m_keeper.value.s32_)>::value) return static_cast<int>(m_keeper.value.s32_);
    return static_cast<int>(m_keeper.value.s64_);
}

unsigned int VariantImpl::toUInt() const
{
    if(!this->isInteger())
        return 0;

    if(std::is_same<unsigned int, decltype(m_keeper.value.u8_)>::value) return static_cast<unsigned int>(m_keeper.value.u8_);
    if(std::is_same<unsigned int, decltype(m_keeper.value.u16_)>::value) return static_cast<unsigned int>(m_keeper.value.u16_);
    if(std::is_same<unsigned int, decltype(m_keeper.value.u32_)>::value) return static_cast<unsigned int>(m_keeper.value.u32_);
    return static_cast<unsigned int>(m_keeper.value.u64_);
}

void *VariantImpl::toPointer() const  { return (m_keeper.type == Variant::Type::POINTER) ? m_keeper.value.pointer : nullptr;  }
Object *VariantImpl::toObject() const { return (m_keeper.type == Variant::Type::OBJECT) ?  m_keeper.value.object  : nullptr;  }
String VariantImpl::toString() const  { return (m_keeper.type == Variant::Type::STRING) ?  *m_keeper.value.string : String(); }

void VariantImpl::set(s8 v)
{
    this->deallocateString();

    m_keeper.type = Variant::Type::S8;
    m_keeper.value.s8_ = v;
}

void VariantImpl::set(s16 v)
{
    this->deallocateString();

    m_keeper.type = Variant::Type::S16;
    m_keeper.value.s16_ = v;
}

void VariantImpl::set(s32 v)
{
    this->deallocateString();

    m_keeper.type = Variant::Type::S32;
    m_keeper.value.s32_ = v;
}

void VariantImpl::set(s64 v)
{
    this->deallocateString();

    m_keeper.type = Variant::Type::S64;
    m_keeper.value.s64_ = v;
}

void VariantImpl::set(u8 v)
{
    this->deallocateString();

    m_keeper.type = Variant::Type::U8;
    m_keeper.value.u8_ = v;
}

void VariantImpl::set(u16 v)
{
    this->deallocateString();

    m_keeper.type = Variant::Type::U16;
    m_keeper.value.u16_ = v;
}

void VariantImpl::set(u32 v)
{
    this->deallocateString();

    m_keeper.type = Variant::Type::U32;
    m_keeper.value.u32_ = v;
}

void VariantImpl::set(u64 v)
{
    this->deallocateString();

    m_keeper.type = Variant::Type::U64;
    m_keeper.value.u64_ = v;
}

void VariantImpl::set(void *v)
{
    this->deallocateString();

    m_keeper.type = Variant::Type::POINTER;
    m_keeper.value.pointer = v;
}

void VariantImpl::set(const String &v)
{
    this->deallocateString();

    m_keeper.type = Variant::Type::STRING;
    m_keeper.value.string = new String(v); // Copy
}

void VariantImpl::set(Object *v)
{
    this->deallocateString();

    m_keeper.type = Variant::Type::OBJECT;
    m_keeper.value.object = v;
}

bool VariantImpl::equals(const Variant &rhs) const
{
    if(m_keeper.type != rhs.pimpl_p()->m_keeper.type)
        return false;

    switch(m_keeper.type)
    {
        case Variant::Type::S8:     return m_keeper.value.s8_  == rhs.pimpl_p()->m_keeper.value.s8_;
        case Variant::Type::S16:    return m_keeper.value.s16_ == rhs.pimpl_p()->m_keeper.value.s16_;
        case Variant::Type::S32:    return m_keeper.value.s32_ == rhs.pimpl_p()->m_keeper.value.s32_;
        case Variant::Type::S64:    return m_keeper.value.s64_ == rhs.pimpl_p()->m_keeper.value.s64_;
        case Variant::Type::U8:     return m_keeper.value.u8_  == rhs.pimpl_p()->m_keeper.value.u8_;
        case Variant::Type::U16:    return m_keeper.value.u16_ == rhs.pimpl_p()->m_keeper.value.u16_;
        case Variant::Type::U32:    return m_keeper.value.u32_ == rhs.pimpl_p()->m_keeper.value.u32_;
        case Variant::Type::U64:    return m_keeper.value.u64_ == rhs.pimpl_p()->m_keeper.value.u64_;
        case Variant::Type::POINTER: return m_keeper.value.pointer == rhs.pimpl_p()->m_keeper.value.pointer;
        case Variant::Type::STRING: return *m_keeper.value.string == *rhs.pimpl_p()->m_keeper.value.string;
        case Variant::Type::OBJECT: return m_keeper.value.object->objectId() == rhs.pimpl_p()->m_keeper.value.object->objectId();
        default: break;
    }

    return false;
}

bool VariantImpl::lt(const Variant &rhs) const
{
    if(m_keeper.type != rhs.pimpl_p()->m_keeper.type)
        return false;

    switch(m_keeper.type)
    {
        case Variant::Type::S8:     return m_keeper.value.s8_  < rhs.pimpl_p()->m_keeper.value.s8_;
        case Variant::Type::S16:    return m_keeper.value.s16_ < rhs.pimpl_p()->m_keeper.value.s16_;
        case Variant::Type::S32:    return m_keeper.value.s32_ < rhs.pimpl_p()->m_keeper.value.s32_;
        case Variant::Type::S64:    return m_keeper.value.s64_ < rhs.pimpl_p()->m_keeper.value.s64_;
        case Variant::Type::U8:     return m_keeper.value.u8_  < rhs.pimpl_p()->m_keeper.value.u8_;
        case Variant::Type::U16:    return m_keeper.value.u16_ < rhs.pimpl_p()->m_keeper.value.u16_;
        case Variant::Type::U32:    return m_keeper.value.u32_ < rhs.pimpl_p()->m_keeper.value.u32_;
        case Variant::Type::U64:    return m_keeper.value.u64_ < rhs.pimpl_p()->m_keeper.value.u64_;
        case Variant::Type::POINTER: return m_keeper.value.pointer < rhs.pimpl_p()->m_keeper.value.pointer;
        case Variant::Type::STRING: return *m_keeper.value.string < *rhs.pimpl_p()->m_keeper.value.string;
        case Variant::Type::OBJECT: return m_keeper.value.object->objectId() < rhs.pimpl_p()->m_keeper.value.object->objectId();
        default: break;
    }

    return false;
}

bool VariantImpl::lte(const Variant &rhs) const
{
    if(m_keeper.type != rhs.pimpl_p()->m_keeper.type)
        return false;

    switch(m_keeper.type)
    {
        case Variant::Type::S8:     return m_keeper.value.s8_  <= rhs.pimpl_p()->m_keeper.value.s8_;
        case Variant::Type::S16:    return m_keeper.value.s16_ <= rhs.pimpl_p()->m_keeper.value.s16_;
        case Variant::Type::S32:    return m_keeper.value.s32_ <= rhs.pimpl_p()->m_keeper.value.s32_;
        case Variant::Type::S64:    return m_keeper.value.s64_ <= rhs.pimpl_p()->m_keeper.value.s64_;
        case Variant::Type::U8:     return m_keeper.value.u8_  <= rhs.pimpl_p()->m_keeper.value.u8_;
        case Variant::Type::U16:    return m_keeper.value.u16_ <= rhs.pimpl_p()->m_keeper.value.u16_;
        case Variant::Type::U32:    return m_keeper.value.u32_ <= rhs.pimpl_p()->m_keeper.value.u32_;
        case Variant::Type::U64:    return m_keeper.value.u64_ <= rhs.pimpl_p()->m_keeper.value.u64_;
        case Variant::Type::POINTER: return m_keeper.value.pointer <= rhs.pimpl_p()->m_keeper.value.pointer;
        case Variant::Type::STRING: return *m_keeper.value.string <= *rhs.pimpl_p()->m_keeper.value.string;
        case Variant::Type::OBJECT: return m_keeper.value.object->objectId() <= rhs.pimpl_p()->m_keeper.value.object->objectId();
        default: break;
    }

    return false;
}

bool VariantImpl::gt(const Variant &rhs) const
{
    if(m_keeper.type != rhs.pimpl_p()->m_keeper.type)
        return false;

    switch(m_keeper.type)
    {
        case Variant::Type::S8:     return m_keeper.value.s8_  > rhs.pimpl_p()->m_keeper.value.s8_;
        case Variant::Type::S16:    return m_keeper.value.s16_ > rhs.pimpl_p()->m_keeper.value.s16_;
        case Variant::Type::S32:    return m_keeper.value.s32_ > rhs.pimpl_p()->m_keeper.value.s32_;
        case Variant::Type::S64:    return m_keeper.value.s64_ > rhs.pimpl_p()->m_keeper.value.s64_;
        case Variant::Type::U8:     return m_keeper.value.u8_  > rhs.pimpl_p()->m_keeper.value.u8_;
        case Variant::Type::U16:    return m_keeper.value.u16_ > rhs.pimpl_p()->m_keeper.value.u16_;
        case Variant::Type::U32:    return m_keeper.value.u32_ > rhs.pimpl_p()->m_keeper.value.u32_;
        case Variant::Type::U64:    return m_keeper.value.u64_ > rhs.pimpl_p()->m_keeper.value.u64_;
        case Variant::Type::POINTER: return m_keeper.value.pointer > rhs.pimpl_p()->m_keeper.value.pointer;
        case Variant::Type::STRING: return *m_keeper.value.string > *rhs.pimpl_p()->m_keeper.value.string;
        case Variant::Type::OBJECT: return m_keeper.value.object->objectId() > rhs.pimpl_p()->m_keeper.value.object->objectId();
        default: break;
    }

    return false;
}

bool VariantImpl::gte(const Variant &rhs) const
{
    if(m_keeper.type != rhs.pimpl_p()->m_keeper.type)
        return false;

    switch(m_keeper.type)
    {
        case Variant::Type::S8:     return m_keeper.value.s8_  >= rhs.pimpl_p()->m_keeper.value.s8_;
        case Variant::Type::S16:    return m_keeper.value.s16_ >= rhs.pimpl_p()->m_keeper.value.s16_;
        case Variant::Type::S32:    return m_keeper.value.s32_ >= rhs.pimpl_p()->m_keeper.value.s32_;
        case Variant::Type::S64:    return m_keeper.value.s64_ >= rhs.pimpl_p()->m_keeper.value.s64_;
        case Variant::Type::U8:     return m_keeper.value.u8_  >= rhs.pimpl_p()->m_keeper.value.u8_;
        case Variant::Type::U16:    return m_keeper.value.u16_ >= rhs.pimpl_p()->m_keeper.value.u16_;
        case Variant::Type::U32:    return m_keeper.value.u32_ >= rhs.pimpl_p()->m_keeper.value.u32_;
        case Variant::Type::U64:    return m_keeper.value.u64_ >= rhs.pimpl_p()->m_keeper.value.u64_;
        case Variant::Type::POINTER: return m_keeper.value.pointer >= rhs.pimpl_p()->m_keeper.value.pointer;
        case Variant::Type::STRING: return *m_keeper.value.string >= *rhs.pimpl_p()->m_keeper.value.string;
        case Variant::Type::OBJECT: return m_keeper.value.object->objectId() >= rhs.pimpl_p()->m_keeper.value.object->objectId();
        default: break;
    }

    return false;
}

void VariantImpl::deallocateString()
{
    if(m_keeper.type == Variant::Type::STRING)
        delete m_keeper.value.string;
}

} // namespace REDasm
