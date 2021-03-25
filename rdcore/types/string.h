#pragma once

#include "type.h"

class StringType: public Type
{
    protected:
        StringType(rd_type type, Context* ctx = nullptr);
        StringType(rd_type type, size_t size, Context* ctx = nullptr);
        StringType(rd_type type, const std::string& name, size_t size, Context* ctx = nullptr);

    public:
        size_t size() const override;
        virtual void calculateSize(rd_address address) = 0;
        bool fromJson(const tao::json::value& v) override;
        tao::json::value toJson() const override;

    protected:
        size_t m_size{1};
};

class AsciiStringType: public StringType
{
    public:
        AsciiStringType(size_t size = RD_NVAL, Context* ctx = nullptr);
        AsciiStringType(const std::string& name, size_t size = RD_NVAL, Context* ctx = nullptr);
        Type* clone(Context* ctx = nullptr) const override;
        void calculateSize(rd_address address) override;
};

class WideStringType: public StringType
{
    public:
        WideStringType(size_t size = RD_NVAL, Context* ctx = nullptr);
        WideStringType(const std::string& name, size_t size = RD_NVAL, Context* ctx = nullptr);
        Type* clone(Context* ctx = nullptr) const override;
        void calculateSize(rd_address address) override;
};
