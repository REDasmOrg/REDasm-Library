#include "rdil.h"
#include "../renderer/renderer.h"
#include "../context.h"
#include <unordered_map>

#define RDIL_N(x) {x, #x}

const char* RDIL::getOpName(rd_type t)
{
    static std::unordered_map<rd_type, const char*> NAMES = {
        RDIL_N(RDIL_Unknown), RDIL_N(RDIL_Nop),
        RDIL_N(RDIL_Reg), RDIL_N(RDIL_Cnst), RDIL_N(RDIL_Addr), RDIL_N(RDIL_Var),
        RDIL_N(RDIL_Add), RDIL_N(RDIL_Sub), RDIL_N(RDIL_Mul), RDIL_N(RDIL_Div),
        RDIL_N(RDIL_And), RDIL_N(RDIL_Or), RDIL_N(RDIL_Xor), RDIL_N(RDIL_Not),
        RDIL_N(RDIL_Load), RDIL_N(RDIL_Store), RDIL_N(RDIL_Copy),
        RDIL_N(RDIL_Goto), RDIL_N(RDIL_Call), RDIL_N(RDIL_Ret),
        RDIL_N(RDIL_If), RDIL_N(RDIL_Eq), RDIL_N(RDIL_Ne),
        RDIL_N(RDIL_Push), RDIL_N(RDIL_Pop)
    };

    auto it = NAMES.find(t);
    return (it != NAMES.end()) ? it->second : "???";
}

void RDIL::render(const ILExpression* e, const Renderer* renderer, RendererItem* ritem, rd_address address)
{
    switch(e->type)
    {
        case RDIL_Copy:
            RDIL::render(e->dst, renderer, ritem, address);
            renderer->renderText(ritem, " = ");
            RDIL::render(e->src, renderer, ritem, address);
            break;

        case RDIL_Call:
            RDIL::render(e->e, renderer, ritem, address);
            renderer->renderText(ritem, "()");
            break;

        case RDIL_Goto:
            renderer->renderMnemonic(ritem, "goto ", Theme_Jump);
            RDIL::render(e->e, renderer, ritem, address);
            break;

        case RDIL_Load:
            renderer->renderText(ritem, "[");
            RDIL::render(e->e, renderer, ritem, address);
            renderer->renderText(ritem, "]");
            break;

        case RDIL_Push:
            renderer->renderMnemonic(ritem, "push", Theme_Default);
            renderer->renderText(ritem, "(");
            RDIL::render(e->e, renderer, ritem, address);
            renderer->renderText(ritem, ")");
            break;

        case RDIL_Pop:
            RDIL::render(e->e, renderer, ritem, address);
            renderer->renderText(ritem, " = ");
            renderer->renderMnemonic(ritem, "pop()", Theme_Default);
            break;

        case RDIL_Add:
        case RDIL_Sub:
        case RDIL_Mul:
        case RDIL_Div:
        case RDIL_And:
        case RDIL_Or:
        case RDIL_Xor:
        case RDIL_Eq:
        case RDIL_Ne:
        case RDIL_Lt:
        case RDIL_Le:
        case RDIL_Gt:
        case RDIL_Ge:
            RDIL::wrap(e->left, renderer, ritem, address);
            renderer->renderText(ritem, RDIL::textOp(e));
            RDIL::render(e->right, renderer, ritem, address);
            break;

        case RDIL_If:
            renderer->renderMnemonic(ritem, "if", Theme_Default);
            renderer->renderText(ritem, "(");
            RDIL::render(e->cond, renderer, ritem, address);
            renderer->renderText(ritem, ") ");
            RDIL::render(e->t, renderer, ritem, address);
            renderer->renderText(ritem, " else ");
            RDIL::render(e->f, renderer, ritem, address);
            break;

        case RDIL_Ret:
            renderer->renderMnemonic(ritem, "ret", Theme_Ret);
            renderer->renderText(ritem, "(");
            RDIL::render(e->cond, renderer, ritem, address);
            renderer->renderText(ritem, ")");
            break;


        case RDIL_Unknown:
            renderer->renderMnemonic(ritem, "unknown ", Theme_Default);
            renderer->renderText(ritem, "{");
            renderer->renderAssemblerInstruction(address, ritem);
            renderer->renderText(ritem, "}");
            break;

        case RDIL_Cnst: renderer->renderUnsigned(ritem, e->u_value); break;
        case RDIL_Addr: renderer->renderUnsigned(ritem, e->u_value); break;
        case RDIL_Var: renderer->renderText(ritem, e->var, Theme_Symbol); break;
        case RDIL_Reg: renderer->renderRegister(ritem, e->reg); break;
        case RDIL_Nop: renderer->renderMnemonic(ritem, "nop ", Theme_Nop); break;
        default: renderer->renderText(ritem, "???"); break;
    }
}

std::string RDIL::textOp(const ILExpression* e)
{
    switch(e->type)
    {
        case RDIL_Add: return " + ";
        case RDIL_Sub: return " - ";
        case RDIL_Mul: return " * ";
        case RDIL_Div: return " / ";
        case RDIL_And: return " & ";
        case RDIL_Or:  return " | ";
        case RDIL_Xor: return " ^ ";
        case RDIL_Eq:  return " == ";
        case RDIL_Ne:  return " != ";
        case RDIL_Lt:  return " < ";
        case RDIL_Le:  return " <= ";
        case RDIL_Gt:  return " > ";
        case RDIL_Ge:  return " >= ";
        default: break;
    }

    return "???";
}

bool RDIL::hasValue(const ILExpression* e)
{
    switch(e->type)
    {
        case RDIL_Var:
        case RDIL_Reg:
        case RDIL_Cnst:
        case RDIL_Addr:
            return true;

        default: break;
    }

    return false;
}

void RDIL::wrap(const ILExpression* e, const Renderer* renderer, RendererItem* ritem, rd_address address)
{
    if(RDIL::hasValue(e))
    {
        RDIL::render(e, renderer, ritem, address);
        return;
    }

    renderer->renderText(ritem, "(");
    RDIL::render(e, renderer, ritem, address);
    renderer->renderText(ritem, ")");
}
