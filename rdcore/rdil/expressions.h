#pragma once

#include <rdapi/rdil.h>
#include <memory>

struct RDILExpression;
typedef std::unique_ptr<RDILExpression> RDILExpressionPtr;

struct RDILExpression {
    ~RDILExpression() {
        if(n3) delete n3;
        if(n2) delete n2;
        if(n1) delete n1;
    }

    rd_type type, rdil{RDIL_Unknown};
    size_t size{0};
    const RDILExpression* parent{nullptr};

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
