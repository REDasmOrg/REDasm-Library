#pragma once

#include <rdapi/types.h>
#include <unordered_map>
#include <deque>
#include <list>
#include <set>
#include "expression.h"
#include "../object.h"

class Disassembler;

class ILFunction: public ILExpressionTree
{
    public:
        typedef std::deque<ILExpression*> ExpressionList;

    public:
        ILFunction(Context* ctx);
        void insert(size_t idx, ILExpression* e);
        void append(ILExpression* e);
        void setCurrentAddress(rd_address address);
        bool getAddress(const ILExpression* e, rd_address* address) const;
        const ILExpression* first() const;
        const ILExpression* last() const;
        const ILExpression* expression(size_t idx) const;
        bool empty() const;
        size_t size() const;

    public:
        static bool generate(rd_address address, ILFunction* il);
        static ILExpression* generateOne(Context* ctx, rd_address address);

    private:
        static bool generatePath(rd_address address, ILFunction* il, std::set<rd_address>& path);
        static void generateBasicBlock(rd_address address, ILFunction* il, std::set<rd_address>& path);

    public:
        ExpressionList::iterator begin();
        ExpressionList::iterator end();
        ExpressionList::const_iterator begin() const;
        ExpressionList::const_iterator end() const;

    private:
        std::unordered_map<const ILExpression*, rd_address> m_addresses;
        rd_address m_currentaddress{RD_NVAL};
        ExpressionList m_expressions;
};
