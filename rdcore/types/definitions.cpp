#include "definitions.h"
#include <numeric>
#include <climits>

#define TYPE_FIELD    "type"
#define SIZE_FIELD    "size"
#define NAME_FIELD    "name"
#define SIGNED_FIELD  "signed"
#define FIELDS_FIELD  "fields"

Type::Type(rd_type t): Object(), m_type(t) { }
rd_type Type::type() const { return m_type; }

Type* Type::load(const tao::json::value& v)
{
    auto* tf = v.find(TYPE_FIELD);
    if(!tf) return nullptr;

    TypePtr t;

    switch(Type::typeId(tf->get_string()))
    {
        case Type_Int: t.reset(new IntType()); break;
        case Type_Float: t.reset(new FloatType()); break;
        case Type_Structure: t.reset(new StructureType()); break;
        default: break;
    }

    if(!t || !t->fromJson(v)) return nullptr;
    return t.release();
}

std::string Type::typeName(rd_type type)
{
    switch(type)
    {
        case Type_Void:      return "void";
        case Type_Bool:      return "bool";
        case Type_Char:      return "char";
        case Type_Int:       return "int";
        case Type_Float:     return "float";
        case Type_Array:     return "array";
        case Type_Function:  return "function";
        case Type_Alias:     return "alias";
        case Type_Structure: return "structure";
        case Type_Union:     return "union";
        case Type_Enum:      return "enum";
        default: break;
    }

    return std::string();
}

rd_type Type::typeId(const std::string& s)
{
    static const std::unordered_map<std::string, rd_type> TYPES = {
        { "void", Type_Void }, { "bool", Type_Bool }, { "char", Type_Char },
        { "int", Type_Int }, { "float", Type_Float }, { "array", Type_Array },
        { "function", Type_Function }, { "alias", Type_Alias },
        { "structure", Type_Structure }, { "union", Type_Union }, { "enum", Type_Enum }
    };

    auto it = TYPES.find(s);
    return (it != TYPES.end()) ? it->second : Type_None;
}

bool Type::fromJson(const tao::json::value& v)
{
    auto* type = v.find(TYPE_FIELD);
    if(!type) return false;

    m_type = Type::typeId(type->get_string());
    if(m_type == Type_None) return false;

    return true;
}

std::string Type::typeName() const { return Type::typeName(m_type); }

tao::json::value Type::toJson() const
{
    return {
        { TYPE_FIELD, Type::typeName(m_type) },
        { SIZE_FIELD, this->size() },
    };
}

StructureType::StructureType(): Type(Type_Structure) { }

Type* StructureType::clone() const
{
    auto* s = new StructureType();
    for(const auto& [n, f] : m_fields) s->append(f->clone(), n);
    return s;
}

size_t StructureType::size() const
{
    return std::accumulate(m_fields.begin(), m_fields.end(), 0, [](int size, const auto& item) {
        return size + item.second->size();
    });
}

void StructureType::append(Type* t, const std::string& name)
{
    if(!t || !t->size() || name.empty()) return;

    auto ns = this->uncollided(name);
    m_byname[ns] = TypePtr(t); // Take ownership
    m_fields.push_back({ns, t});
}

bool StructureType::fromJson(const tao::json::value& v)
{
    if(!Type::fromJson(v)) return false;

    auto* f = v.find(FIELDS_FIELD);
    if(!f || !f->is_array()) return false;

    for(const auto& fi : f->get_array())
    {
        auto& obj = fi.get_object();

        this->append(Type::load(obj.at(TYPE_FIELD).get_object()),
                     obj.at(NAME_FIELD).get_string());
    }

    return true;
}

tao::json::value StructureType::toJson() const
{
    auto obj = Type::toJson();
    obj[FIELDS_FIELD] = tao::json::empty_array;

    for(const auto& item : m_fields)
    {
        tao::json::value f = tao::json::empty_object;
        f[NAME_FIELD] = item.first;
        f[TYPE_FIELD] = item.second->toJson();
        obj[FIELDS_FIELD].push_back(f);
    }

    return obj;
}

const StructureType::Fields& StructureType::fields() const { return m_fields; }
StructureType::Fields::const_iterator StructureType::begin() const { return m_fields.begin(); }
StructureType::Fields::const_iterator StructureType::end() const { return m_fields.end(); }
StructureType::Fields::iterator StructureType::begin() { return m_fields.begin(); }
StructureType::Fields::iterator StructureType::end() { return m_fields.end(); }

std::string StructureType::uncollided(std::string s) const
{
    std::string n = s;

    for(int i = 0; m_byname.count(s); i++)
        s = n + "_" + std::to_string(i);

    return s;
}

NumericType::NumericType(rd_type type): Type(type) { }
NumericType::NumericType(rd_type type, size_t size, bool issigned): Type(type), m_size(size), m_signed(issigned) { }

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

    auto* size = v.find(SIZE_FIELD);
    if(!size) return false;
    size->to(m_size);

    auto* sign = v.find(SIGNED_FIELD);
    if(!sign) return false;
    sign->to(m_signed);

    return true;
}

tao::json::value NumericType::toJson() const
{
    auto obj = Type::toJson();
    obj[SIZE_FIELD] = m_size;
    obj[SIGNED_FIELD] = m_signed;
    return obj;
}

bool NumericType::isSigned() const { return m_signed; }

IntType::IntType(): NumericType(Type_Int) { }
IntType::IntType(size_t size, bool issigned): NumericType(Type_Int, size, issigned) { }
Type* IntType::clone() const { return new IntType(this->size(), this->isSigned()); }

FloatType::FloatType(): NumericType(Type_Float) { }
FloatType::FloatType(size_t size, bool issigned): NumericType(Type_Float, size, issigned) { }
Type* FloatType::clone() const { return new FloatType(this->size(), this->isSigned()); }
