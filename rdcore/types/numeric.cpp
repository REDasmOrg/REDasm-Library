#include "numeric.h"
#include <climits>

NumericType::NumericType(rd_type type, Context* ctx): Type(type, ctx) { }
NumericType::NumericType(rd_type type, const std::string& name, size_t size, bool issigned, Context* ctx): Type(type, name, ctx), m_size(size), m_signed(issigned) { }

std::string NumericType::typeName() const
{
    std::string tn;
    tn += m_signed ? "s" : "u";
    tn += std::to_string(m_size * CHAR_BIT);
    return tn;
}

size_t NumericType::size() const { return m_size; }

bool NumericType::fromJson(const tao::json::value& v)
{
    if(!Type::fromJson(v)) return false;

    auto* size = v.find(T_SIZE_FIELD);
    if(!size) return false;
    size->to(m_size);

    auto* sign = v.find(T_SIGNED_FIELD);
    if(!sign) return false;
    sign->to(m_signed);

    return true;
}

tao::json::value NumericType::toJson() const
{
    auto obj = Type::toJson();
    obj[T_SIZE_FIELD] = m_size;
    obj[T_SIGNED_FIELD] = m_signed;
    return obj;
}

bool NumericType::isSigned() const { return m_signed; }

IntType::IntType(Context* ctx): NumericType(Type_Int, ctx) { }
IntType::IntType(size_t size, bool issigned, Context* ctx): NumericType(Type_Int, std::string(), size, issigned, ctx) { }
IntType::IntType(size_t size, const std::string& name, bool issigned, Context* ctx): NumericType(Type_Int, name, size, issigned, ctx) { }
Type* IntType::clone(Context* ctx) const { return new IntType(this->size(), this->name(), this->isSigned(), ctx); }

FloatType::FloatType(Context* ctx): NumericType(Type_Float, ctx) { }
FloatType::FloatType(size_t size, const std::string& name, bool issigned, Context* ctx): NumericType(Type_Float, name, size, issigned, ctx) { }
FloatType::FloatType(size_t size, bool issigned, Context* ctx): NumericType(Type_Float, std::string(), size, issigned, ctx) { }
Type* FloatType::clone(Context* ctx) const { return new FloatType(this->size(), this->name(), this->isSigned(), ctx); }
