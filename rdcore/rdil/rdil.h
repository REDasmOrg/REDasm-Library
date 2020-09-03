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
        static void render(const ILExpression* expr, const Renderer* renderer, RendererItem* ritem, rd_address address);
        static bool hasValue(const ILExpression* e);
        static bool match(const ILExpression* e, const char* m);

    private:
        static bool match(const ILExpression* e, std::string& res);
        static std::string textOp(const ILExpression* e);
        static void wrap(const ILExpression* e, const Renderer* renderer, RendererItem* ritem, rd_address address);
        static void wrapMatch(const ILExpression* e, std::string& res);
};
