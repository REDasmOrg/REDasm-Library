#pragma once

#include <rdapi/database/types.h>
#include <rdapi/types.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <type_traits>
#include "../object.h"

class Type: public Object
{
    protected:
        Type(rd_type t);

    public:
        const std::string& name() const;
        virtual Type* clone() const = 0;
        virtual size_t size() const = 0;
        rd_type type() const;

    private:
        std::string m_name;
        rd_type m_type;
};

typedef std::unique_ptr<Type> TypePtr;

class NumericType: public Type
{
    protected:
        NumericType(rd_type type, size_t bits, bool issigned);

    public:
        bool isSigned() const;
        size_t size() const override;

    private:
        size_t m_bits{0};
        bool m_signed{false};
};

class IntType: public NumericType
{
    public:
        IntType(size_t bits, bool issigned);
        Type* clone() const override;
};

class FloatType: public NumericType
{
    public:
        FloatType(size_t bits, bool issigned);
        Type* clone() const override;
};

class StructureType: public Type
{
    private:
        typedef std::unordered_map<std::string, TypePtr> FieldsByName;
        typedef std::vector<std::pair<std::string, Type*>> Fields;

    public:
        StructureType();
        Type* clone() const override;
        size_t size() const override;
        void append(Type* t, const std::string& name);

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
        size_t m_size{0};
};
