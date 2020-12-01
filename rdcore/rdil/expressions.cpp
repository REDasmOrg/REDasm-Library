#include "expressions.h"

ILExpression* ILExpression::clone(const ILExpression* e)
{
    if(!e) return nullptr;
    auto* ne = new ILExpression();
    ne->type = e->type;
    ne->size = e->size;
    ne->value = e->value;

    if(e->n1)
    {
        auto it = ne->children.emplace(ILExpression::clone(e->n1));
        ne->n1 = it.first->get();
    }

    if(e->n2)
    {
        auto it = ne->children.emplace(ILExpression::clone(e->n2));
        ne->n2 = it.first->get();
    }

    if(e->n3)
    {
        auto it = ne->children.emplace(ILExpression::clone(e->n3));
        ne->n3 = it.first->get();
    }

    return ne;
}
