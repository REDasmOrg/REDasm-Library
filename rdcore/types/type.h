#pragma once

#define T_TYPE_FIELD     "type"
#define T_SIZE_FIELD     "size"
#define T_NAME_FIELD     "name"
#define T_ITEMTYPE_FIELD "itemtype"
#define T_SIGNED_FIELD   "signed"
#define T_FIELDS_FIELD   "fields"

#include <rdapi/database/types.h>
#include <rdapi/types.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <type_traits>
#include <tao/json.hpp>
#include "../object.h"

class Type: public Object
{
    protected:
        Type(rd_type t, Context* ctx = nullptr);
        Type(rd_type t, const std::string& name, Context* ctx = nullptr);

    public:
        virtual bool fromJson(const tao::json::value& v);
        virtual std::string typeName() const;
        virtual tao::json::value toJson() const;
        virtual Type* clone(Context* ctx = nullptr) const = 0;
        virtual size_t size() const = 0;
        size_t bits() const;
        void setName(const std::string& name);
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
