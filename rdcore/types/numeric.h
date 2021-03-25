#pragma once

#include "type.h"

class NumericType: public Type
{
    protected:
        NumericType(rd_type type, Context* ctx = nullptr);
        NumericType(rd_type type, const std::string& name, size_t size, bool issigned, Context* ctx = nullptr);
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
        IntType(Context* ctx = nullptr);
        IntType(size_t size, bool issigned, Context* ctx = nullptr);
        IntType(size_t size, const std::string& name, bool issigned, Context* ctx = nullptr);
        Type* clone(Context* ctx = nullptr) const override;
};

class FloatType: public NumericType
{
    public:
        FloatType(Context* ctx = nullptr);
        FloatType(size_t size, bool issigned, Context* ctx = nullptr);
        FloatType(size_t size, const std::string& name, bool issigned, Context* ctx = nullptr);
        Type* clone(Context* ctx = nullptr) const override;
};
