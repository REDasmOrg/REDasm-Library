#pragma once

#include <rdapi/rdil.h>
#include <memory>
#include <unordered_set>
#include "../object.h"

class ILExpression;
typedef std::unique_ptr<ILExpression> ILExpressionPtr;

class ILExpression : public Object
{
    public:
        rd_type type{RDIL_Unknown};
        size_t size{0};

        union { ILExpression *n1{nullptr}, *u, *cond; };
        union { ILExpression *n2{nullptr}, *dst, *left, *t; };
        union { ILExpression *n3{nullptr}, *src, *right, *f; };
        union { PRIVATE_RDIL_VALUE_FIELDS };

    public:
        static ILExpression* clone(const ILExpression* e);

    private:
        std::unordered_set<ILExpressionPtr> children; // Keep expressions alive, if needed

    friend class ILFunction;
};
