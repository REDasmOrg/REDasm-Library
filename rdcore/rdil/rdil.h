#pragma once

#include <rdapi/rdil.h>
#include <functional>
#include <string>
#include <queue>
#include "expression.h"

class Renderer;

class RDIL
{
    private:
        enum class WalkType { Normal = 0, Mnemonic, Whitespace, };
        typedef std::function<void(const ILExpression* e, const std::string& s, WalkType wt)> WalkCallback;
        struct RDILQueryItem { std::string nodeid, opcode; };
        typedef std::queue<RDILQueryItem> RDILQuery;

    public:
        RDIL() = delete;
        static const char* getOpName(rd_type t);
        static rd_type getOpType(const std::string& id);
        static std::string getText(const ILExpression* e);
        static void render(const ILExpression* e, Renderer* renderer, rd_address address);
        static const ILExpression* extract(const ILExpression* e, const char* q);
        static bool match(const ILExpression* e, const char* m);
        static bool hasValue(const ILExpression* e);

    private:
        static const ILExpression* extract(const ILExpression* e, const RDILQueryItem& qi, RDILQuery& query, int depth = 0);
        static void walk(const ILExpression* e, const WalkCallback& cb);
        static bool match(const ILExpression* e, std::string& res);
        static bool parseQuery(const std::string& q, RDILQuery& query);
        static void wrapWalk(const ILExpression* e, const WalkCallback& cb);
        static void wrapMatch(const ILExpression* e, std::string& res);
        static void getText(const ILExpression* e, std::string& res);
        static std::string textOp(const ILExpression* e);
};
