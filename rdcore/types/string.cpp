#include "string.h"
#include "../disassembler.h"
#include "../context.h"

StringType::StringType(rd_type type, Context* ctx): Type(type, ctx) { }
StringType::StringType(rd_type type, size_t size, Context* ctx): Type(type, ctx), m_size(size == RD_NVAL ? 1 : size) { }
size_t StringType::size() const { return m_size; }

bool StringType::fromJson(const tao::json::value& v)
{
    if(!Type::fromJson(v)) return false;

    auto* size = v.find(T_SIZE_FIELD);
    if(!size) return false;
    size->to(m_size);

    return true;
}

tao::json::value StringType::toJson() const
{
    auto obj = Type::toJson();
    obj[T_SIZE_FIELD] = m_size;
    return obj;
}

AsciiStringType::AsciiStringType(size_t size, Context* ctx): StringType(Type_AsciiString, size, ctx) { }
Type* AsciiStringType::clone(Context* ctx) const { return new AsciiStringType(m_size, ctx); }

void AsciiStringType::calculateSize(rd_address address)
{
    if(m_size > 1) return;
    std::string s = this->context()->disassembler()->readString(address);
    if(!s.empty()) m_size = s.size();
}

WideStringType::WideStringType(size_t size, Context* ctx): StringType(Type_WideString, size, ctx) { }
Type* WideStringType::clone(Context* ctx) const { return new WideStringType(m_size, ctx); }

void WideStringType::calculateSize(rd_address address)
{
    if(m_size > 1) return;
    std::string s = this->context()->disassembler()->readWString(address);
    if(!s.empty()) m_size = s.size();
}
