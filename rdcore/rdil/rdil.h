#pragma once

#include <rdapi/rdil.h>
#include <functional>
#include <optional>
#include <string>
#include "expression.h"
#include "ilfunction.h"

class Renderer;

class RDIL
{
    private:
        enum class WalkType { Normal = 0, Mnemonic, Whitespace, };
        typedef std::function<void(const ILExpression* e)> WalkCallback;
        typedef std::function<void(const ILExpression* e, const std::string& s, WalkType wt)> ToStringCallback;
        typedef std::function<void(const ILExpression* e)> FormatCallback;
        struct RDILQueryItem { std::string nodeid, opcode; };

    public:
        RDIL() = delete;
        static const char* getOpName(rd_type t);
        static rd_type getOpType(const std::string& id);
        static std::string getText(const ILExpression* e);
        static std::string getFormat(const ILExpression* e);
        static void render(const ILExpression* e, Renderer* renderer, rd_address address);
        static size_t extract(const ILExpression* e, const RDILValue** values);
        static size_t extract(const ILFunction* f, const RDILValue** values);
        static bool match(const ILFunction* f, const char* m);
        static bool match(const ILExpression* e, const char* m);
        static bool hasValue(const ILExpression* e);

    private:
        static void extract(const ILExpression* e, std::vector<RDILValue>& values);
        static void toString(const ILExpression* e, const ToStringCallback& cb);
        static void walk(const ILExpression* e, const WalkCallback& cb);
        static bool format(const ILExpression* e, std::string& res);
        static void wrapWalk(const ILExpression* e, const ToStringCallback& cb);
        static void wrapFormat(const ILExpression* e, std::string& res);
        static void getText(const ILExpression* e, std::string& res);
        static std::string textOp(const ILExpression* e);
};
