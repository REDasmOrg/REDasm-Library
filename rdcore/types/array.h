#pragma once

#include "type.h"

class ArrayType: public Type
{
    public:
        ArrayType();
        ArrayType(size_t itemscount, Type* type, Context* ctx = nullptr);
        ArrayType(size_t itemscount, Type* type, const std::string& name, Context* ctx = nullptr);
        size_t size() const override;
        size_t itemsCount() const;
        const Type* type() const;
        Type* clone(Context* ctx = nullptr) const override;
        bool fromJson(const tao::json::value& v) override;
        tao::json::value toJson() const override;

    private:
        TypePtr m_type;
        size_t m_itemscount;
};
