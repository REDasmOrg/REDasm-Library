#pragma once

#include <rdapi/rdil.h>
#include <memory>

struct RDILExpression;
typedef std::unique_ptr<RDILExpression> RDILExpressionPtr;

struct RDILExpression {
    rd_type type{RDIL_Unknown};
    size_t size{0};

    union { RDILExpression *n1{nullptr}, *e, *cond; };
    union { RDILExpression *n2{nullptr}, *dst, *left, *t; };
    union { RDILExpression *n3{nullptr}, *src, *right, *f; };

    union {
        uintptr_t value{0};
        rd_address address;
        rd_offset offset;
        rd_location location;
        u64 u_value;
        s64 s_value;
        const char* reg;
        const char* var;
    };
};
