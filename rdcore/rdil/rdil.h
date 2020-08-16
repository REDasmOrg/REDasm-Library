#pragma once

#include <rdapi/rdil.h>
#include <functional>
#include <string>
#include "expressions.h"

class RendererItem;
class Renderer;

class RDIL
{
    private:
        typedef std::function<void(const RDILExpression*)> WalkCallback;

    public:
        RDIL() = delete;
        static const char* getOpName(rd_type t);
        static void render(const RDILExpression* expr, const Renderer* renderer, RendererItem* ritem, rd_address address);
        static bool isLeaf(const RDILExpression* e);

    private:
        static std::string textOp(const RDILExpression* e);
        static void wrap(const RDILExpression* e, const Renderer* renderer, RendererItem* ritem, rd_address address);
};
