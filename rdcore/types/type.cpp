#include "type.h"
#include "numeric.h"
#include "string.h"
#include "structure.h"

Type::Type(rd_type t, Context* ctx): Object(ctx), m_type(t) { }
Type::Type(rd_type t, const std::string& name, Context* ctx): Object(ctx), m_type(t), m_name(name) { }
rd_type Type::type() const { return m_type; }

Type* Type::load(const tao::json::value& v)
{
    auto* tf = v.find(T_TYPE_FIELD);
    if(!tf) return nullptr;

    TypePtr t;

    switch(Type::typeId(tf->get_string()))
    {
        case Type_Int: t.reset(new IntType()); break;
        case Type_Float: t.reset(new FloatType()); break;
        case Type_Structure: t.reset(new StructureType()); break;
        case Type_AsciiString: t.reset(new AsciiStringType()); break;
        case Type_WideString: t.reset(new WideStringType()); break;
        default: break;
    }

    if(!t || !t->fromJson(v)) return nullptr;
    return t.release();
}

std::string Type::typeName(rd_type type)
{
    switch(type)
    {
        case Type_Void:        return "void";
        case Type_Bool:        return "bool";
        case Type_Char:        return "char";
        case Type_Int:         return "int";
        case Type_Float:       return "float";
        case Type_AsciiString: return "string";
        case Type_WideString:  return "wstring";
        case Type_Array:       return "array";
        case Type_Function:    return "function";
        case Type_Alias:       return "alias";
        case Type_Structure:   return "structure";
        case Type_Union:       return "union";
        case Type_Enum:        return "enum";
        default: break;
    }

    return std::string();
}

rd_type Type::typeId(const std::string& s)
{
    static const std::unordered_map<std::string, rd_type> TYPES = {
        { "void", Type_Void }, { "bool", Type_Bool }, { "char", Type_Char },
        { "int", Type_Int }, { "float", Type_Float },
        { "string", Type_AsciiString }, { "wstring", Type_WideString },
        { "array", Type_Array },
        { "function", Type_Function }, { "alias", Type_Alias },
        { "structure", Type_Structure }, { "union", Type_Union }, { "enum", Type_Enum }
    };

    auto it = TYPES.find(s);
    return (it != TYPES.end()) ? it->second : Type_None;
}

bool Type::fromJson(const tao::json::value& v)
{
    auto* type = v.find(T_TYPE_FIELD);
    if(!type) return false;

    auto* name = v.find(T_NAME_FIELD);
    if(name) name->to(m_name);

    m_type = Type::typeId(type->get_string());
    if(m_type == Type_None) return false;

    return true;
}

std::string Type::typeName() const { return Type::typeName(m_type); }

tao::json::value Type::toJson() const
{
    return {
        { T_TYPE_FIELD, Type::typeName(m_type) },
        { T_SIZE_FIELD, this->size() },
        { T_NAME_FIELD, this->name() },
    };
}

void Type::setName(const std::string& name) { m_name = name; }
const std::string& Type::name() const { return m_name; }
