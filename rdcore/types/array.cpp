#include "array.h"

ArrayType::ArrayType(): Type(Type_Array), m_itemscount(0) { }
ArrayType::ArrayType(size_t itemscount, Type* type, Context* ctx): Type(Type_Array, ctx), m_type(type), m_itemscount(itemscount) { type->setContext(ctx); }
size_t ArrayType::size() const { return m_type ? (m_itemscount * m_type->size()) : 0; }
size_t ArrayType::itemsCount() const { return m_itemscount; }
const Type* ArrayType::type() const { return m_type.get(); }
Type* ArrayType::clone(Context* ctx) const { return new ArrayType(m_itemscount, m_type->clone(), ctx); }

bool ArrayType::fromJson(const tao::json::value& v)
{
    if(!Type::fromJson(v)) return false;

    auto* size = v.find(T_SIZE_FIELD);
    if(!size) return false;
    size->to(m_itemscount);

    auto* itemtype = v.find(T_ITEMTYPE_FIELD);
    if(!itemtype) return false;
    m_type.reset(Type::load(*itemtype));
    return true;
}

tao::json::value ArrayType::toJson() const
{
    auto obj = Type::toJson();
    obj[T_SIZE_FIELD] = m_itemscount;
    obj[T_ITEMTYPE_FIELD] = m_type->toJson();
    return obj;
}
