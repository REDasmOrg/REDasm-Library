#pragma once

#include "type.h"

class StructureType: public Type
{
    private:
        typedef std::unordered_map<std::string, TypePtr> FieldsByName;
        typedef std::vector<std::pair<std::string, Type*>> Fields;

    public:
        StructureType();
        StructureType(const std::string& name);
        Type* clone(Context* = nullptr) const override;
        size_t size() const override;
        void append(Type* t, const std::string& name);
        bool fromJson(const tao::json::value& v) override;
        tao::json::value toJson() const override;
        const Fields& fields() const;

    public: // C++ Interface
        Fields::const_iterator begin() const;
        Fields::const_iterator end() const;
        Fields::iterator begin();
        Fields::iterator end();

    private:
        std::string uncollided(std::string s) const;

    private:
        FieldsByName m_byname;
        Fields m_fields;
};
