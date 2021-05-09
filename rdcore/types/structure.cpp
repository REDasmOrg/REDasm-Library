#include "structure.h"
#include <numeric>

StructureType::StructureType(): Type(Type_Structure) { }
StructureType::StructureType(const std::string& name): Type(Type_Structure, name) { }

Type* StructureType::clone(Context*) const
{
    auto* s = new StructureType(this->name());
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
    t->setName(ns);
    m_byname[ns] = TypePtr(t); // Take ownership
    m_fields.push_back({ns, t});
}

bool StructureType::fromJson(const tao::json::value& v)
{
    if(!Type::fromJson(v)) return false;

    auto* f = v.find(T_FIELDS_FIELD);
    if(!f || !f->is_array()) return false;

    for(const auto& fi : f->get_array())
    {
        auto& obj = fi.get_object();

        this->append(Type::load(obj.at(T_TYPE_FIELD).get_object()),
                     obj.at(T_NAME_FIELD).get_string());
    }

    return true;
}

tao::json::value StructureType::toJson() const
{
    auto obj = Type::toJson();
    obj[T_FIELDS_FIELD] = tao::json::empty_array;

    for(const auto& item : m_fields)
    {
        tao::json::value f = tao::json::empty_object;
        f[T_NAME_FIELD] = item.first;
        f[T_TYPE_FIELD] = item.second->toJson();
        obj[T_FIELDS_FIELD].push_back(f);
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
