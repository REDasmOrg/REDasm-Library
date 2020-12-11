#include "definitions.h"
#include <climits>

Type::Type(rd_type t): Object(), m_type(t) { }
rd_type Type::type() const { return m_type; }
const std::string& Type::name() const { return m_name; }

StructureType::StructureType(): Type(Type_Structure) { }

Type* StructureType::clone() const
{
    auto* s = new StructureType();
    for(const auto& [n, f] : m_fields) s->append(f->clone(), n);
    return s;
}

size_t StructureType::size() const { return m_size; }

void StructureType::append(Type* t, const std::string& name)
{
    if(!t || !t->size() || name.empty()) return;

    auto ns = this->uncollided(name);
    m_size += t->size();
    m_byname[ns] = TypePtr(t); // Take ownership
    m_fields.push_back({ns, t});
}

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

NumericType::NumericType(rd_type type, size_t bits, bool issigned): Type(type), m_bits(bits), m_signed(issigned) { }
size_t NumericType::size() const { return m_bits / CHAR_BIT; }
bool NumericType::isSigned() const { return m_signed; }

IntType::IntType(size_t bsize, bool issigned): NumericType(Type_Int, bsize, issigned) { }
Type* IntType::clone() const { return new IntType(this->size(), this->isSigned()); }

FloatType::FloatType(size_t bsize, bool issigned): NumericType(Type_Float, bsize, issigned) { }
Type* FloatType::clone() const { return new FloatType(this->size(), this->isSigned()); }
