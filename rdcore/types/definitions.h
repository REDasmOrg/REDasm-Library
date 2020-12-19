#pragma once

#include <rdapi/database/types.h>
#include <rdapi/types.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <type_traits>
#include "../libs/tao/json.hpp"
#include "../object.h"

class Type: public Object
{
    protected:
        Type(rd_type t);
        Type(rd_type t, const std::string& name);

    public:
        virtual bool fromJson(const tao::json::value& v);
        virtual std::string typeName() const;
        virtual tao::json::value toJson() const;
        virtual Type* clone() const = 0;
        virtual size_t size() const = 0;
        const std::string& name() const;
        rd_type type() const;

    public:
        static Type* load(const tao::json::value& v);

    public:
        static std::string typeName(rd_type type);
        static rd_type typeId(const std::string& s);

    private:
        rd_type m_type;
        std::string m_name;
};

typedef std::unique_ptr<Type> TypePtr;

class NumericType: public Type
{
    protected:
        NumericType(rd_type type);
        NumericType(rd_type type, size_t size, bool issigned);
        std::string typeName() const override;

    public:
        bool isSigned() const;
        size_t size() const override;
        bool fromJson(const tao::json::value& v) override;
        tao::json::value toJson() const override;

    private:
        size_t m_size{0};
        bool m_signed{false};
};

class IntType: public NumericType
{
    public:
        IntType();
        IntType(size_t size, bool issigned);
        Type* clone() const override;
};

class FloatType: public NumericType
{
    public:
        FloatType();
        FloatType(size_t size, bool issigned);
        Type* clone() const override;
};

class StructureType: public Type
{
    private:
        typedef std::unordered_map<std::string, TypePtr> FieldsByName;
        typedef std::vector<std::pair<std::string, Type*>> Fields;

    public:
        StructureType();
        StructureType(const std::string& name);
        Type* clone() const override;
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
        size_t m_size{0};
};
