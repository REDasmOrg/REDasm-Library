#include "rdil.h"
#include "../surface/renderer.h"
#include "../support/utils.h"
#include "../support/error.h"
#include "../support/lexer.h"
#include "../config.h"
#include <unordered_map>
#include <string_view>
#include <vector>

#define RDIL_N(x) {x, #x}

const char* RDIL::getOpName(rd_type t)
{
    static const std::unordered_map<rd_type, const char*> NAMES = {
        RDIL_N(RDIL_Unknown), RDIL_N(RDIL_Nop),
        RDIL_N(RDIL_Reg), RDIL_N(RDIL_Cnst), RDIL_N(RDIL_Var),
        RDIL_N(RDIL_Add), RDIL_N(RDIL_Sub), RDIL_N(RDIL_Mul), RDIL_N(RDIL_Div), RDIL_N(RDIL_Mod),
        RDIL_N(RDIL_And), RDIL_N(RDIL_Or), RDIL_N(RDIL_Xor), RDIL_N(RDIL_Not),
        RDIL_N(RDIL_Lsl), RDIL_N(RDIL_Lsr), RDIL_N(RDIL_Asl), RDIL_N(RDIL_Asr),
        RDIL_N(RDIL_Rol), RDIL_N(RDIL_Ror),
        RDIL_N(RDIL_Mem), RDIL_N(RDIL_Copy),
        RDIL_N(RDIL_Goto), RDIL_N(RDIL_Call), RDIL_N(RDIL_Ret),
        RDIL_N(RDIL_If), RDIL_N(RDIL_Eq), RDIL_N(RDIL_Ne),
        RDIL_N(RDIL_Push), RDIL_N(RDIL_Pop)
    };

    auto it = NAMES.find(t);
    return (it != NAMES.end()) ? it->second : "???";
}

rd_type RDIL::getOpType(const std::string& id)
{
    static const std::unordered_map<std::string, rd_type> IDS = {
        { "unknown", RDIL_Unknown }, { "nop", RDIL_Nop },
        { "reg", RDIL_Reg }, { "cnst", RDIL_Cnst }, { "var", RDIL_Var },
        { "add", RDIL_Add }, { "sub", RDIL_Sub }, { "mul", RDIL_Mul }, { "div", RDIL_Div }, { "mod", RDIL_Mod },
        { "and", RDIL_And }, { "or", RDIL_Or }, { "xor", RDIL_Xor }, { "not", RDIL_Not },
        { "lsl", RDIL_Lsl }, { "lsr", RDIL_Lsr }, { "asl", RDIL_Asl }, { "asr", RDIL_Asr },
        { "rol", RDIL_Rol }, { "ror", RDIL_Ror },
        { "mem", RDIL_Mem }, { "copy", RDIL_Copy },
        { "goto", RDIL_Goto }, { "call", RDIL_Call }, { "ret", RDIL_Ret },
        { "eq", RDIL_Eq }, { "ne", RDIL_Ne },
        { "push", RDIL_Push }, { "pop", RDIL_Pop }
    };

    auto it = IDS.find(id);
    return (it != IDS.end()) ? it->second : static_cast<rd_type>(RD_NVAL);
}

std::string RDIL::getText(const ILExpression* e)
{
    std::string res;
    RDIL::getText(e, res);
    return res;
}

std::string RDIL::getFormat(const ILExpression* e)
{
    if(!e) return std::string();

    std::string fmt;
    if(!RDIL::format(e, fmt)) return std::string();
    return fmt;
}

void RDIL::render(const ILExpression* e, Renderer* renderer, rd_address address)
{
    RDIL::toString(e, [renderer, address](const ILExpression* expr, const std::string& s, WalkType wt) {
        if(wt == WalkType::Mnemonic) {
            switch(expr->type) {
                case RDIL_Goto: renderer->renderMnemonic(s, Theme_Jump); break;
                case RDIL_Ret: renderer->renderMnemonic(s, Theme_Ret); break;
                case RDIL_Nop: renderer->renderMnemonic(s, Theme_Nop); break;

                case RDIL_Unknown: {
                    renderer->renderMnemonic(s, Theme_Default);

                    if(address != RD_NVAL) {
                        renderer->renderText(" {");
                        renderer->renderAssemblerInstruction();
                        renderer->renderText("}");
                    }

                    break;
                }

                default: renderer->renderMnemonic(s, Theme_Default); break;
            }

            return;
        }

        switch(expr->type) {
            case RDIL_Cnst: renderer->renderReference(expr->u_value); break;
            case RDIL_Var:  renderer->renderText(expr->var, Theme_Label); break;
            case RDIL_Reg:  renderer->renderRegister(expr->reg); break;
            default: renderer->renderText(s); break;
        }
    });
}

size_t RDIL::extract(const ILExpression* e, const RDILValue** values)
{
    if(!e) return false;

    static std::vector<RDILValue> v;
    v = { };

    RDIL::extract(e, v);
    if(values) *values = v.data();
    return v.size();
}

size_t RDIL::extract(const ILFunction* f, const RDILValue** values)
{
    if(!f) return false;

    static std::vector<RDILValue> v;
    v = { };

    for(const ILExpression* e : *f)
        RDIL::extract(e, v);

    if(values) *values = v.data();
    return v.size();
}

bool RDIL::match(const ILFunction* f, const char* m)
{
    if(!f || !m) return false;

    auto parts = Utils::split(m, ';');
    if(parts.size() != f->size()) return false;

    for(size_t i = 0; i < parts.size(); i++)
    {
        if(!RDIL::match(f->expression(i), parts.at(i).c_str()))
            return false;
    }

    return true;
}

std::string RDIL::textOp(const ILExpression* e)
{
    switch(e->type)
    {
        case RDIL_Add: return "+";
        case RDIL_Sub: return "-";
        case RDIL_Mul: return "*";
        case RDIL_Div: return "/";
        case RDIL_Mod: return "%";
        case RDIL_And: return "&";
        case RDIL_Or:  return "|";
        case RDIL_Xor: return "^";
        case RDIL_Not: return "~";
        case RDIL_Lsl: return "<<";
        case RDIL_Lsr: return ">>";
        case RDIL_Asl: return "<<<";
        case RDIL_Asr: return ">>>";
        case RDIL_Eq:  return "==";
        case RDIL_Ne:  return "!=";
        case RDIL_Lt:  return "<";
        case RDIL_Le:  return "<=";
        case RDIL_Gt:  return ">";
        case RDIL_Ge:  return ">=";
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
            return true;

        default: break;
    }

    return false;
}

void RDIL::extract(const ILExpression* e, std::vector<RDILValue>& values)
{
    RDIL::walk(e, [&values](const ILExpression* expr) {
        if(RDIL::hasValue(expr)) values.emplace_back(RDILValue{expr->type, {expr->value}});
    });
}

void RDIL::toString(const ILExpression* e, const RDIL::ToStringCallback& cb)
{
    switch(e->type)
    {
        case RDIL_Add:
        case RDIL_Sub:
        case RDIL_Mul:
        case RDIL_Div:
        case RDIL_Mod:
        case RDIL_And:
        case RDIL_Or:
        case RDIL_Xor:
        case RDIL_Lsl:
        case RDIL_Lsr:
        case RDIL_Asl:
        case RDIL_Asr:
        case RDIL_Eq:
        case RDIL_Ne:
        case RDIL_Lt:
        case RDIL_Le:
        case RDIL_Gt:
        case RDIL_Ge:
            RDIL::wrapWalk(e->left, cb);
            cb(e, RDIL::textOp(e), WalkType::Normal);
            RDIL::toString(e->right, cb);
            break;

        case RDIL_Rol:
        case RDIL_Ror:
            cb(e, (e->type == RDIL_Rol) ? "rol" : "ror", WalkType::Mnemonic);
            cb(e, "(", WalkType::Normal);
            RDIL::toString(e->left, cb);
            cb(e, ", ", WalkType::Normal);
            RDIL::toString(e->right, cb);
            cb(e, ")", WalkType::Normal);
            break;

        case RDIL_Not:
            cb(e, RDIL::textOp(e), WalkType::Normal);
            RDIL::wrapWalk(e->u, cb);
            break;

        case RDIL_If:
            cb(e, "if", WalkType::Mnemonic);
            cb(e, " ", WalkType::Normal);
            cb(e, "(", WalkType::Normal);
            RDIL::toString(e->cond, cb);
            cb(e, ") ", WalkType::Normal);
            RDIL::toString(e->t, cb);
            cb(e, " ", WalkType::Whitespace);
            cb(e, "else", WalkType::Mnemonic);
            cb(e, " ", WalkType::Whitespace);
            RDIL::toString(e->f, cb);
            break;

        case RDIL_Ret:
            cb(e, "ret", WalkType::Mnemonic);
            cb(e, "(", WalkType::Normal);
            RDIL::toString(e->cond, cb);
            cb(e, ")", WalkType::Normal);
            break;

        case RDIL_Cnst:
        case RDIL_Var:
        case RDIL_Reg:
            cb(e, std::string(), WalkType::Normal);
            break;

        case RDIL_Copy:
            RDIL::toString(e->dst, cb);
            cb(e, "=", WalkType::Normal);
            RDIL::toString(e->src, cb);
            break;

        case RDIL_Call:
            RDIL::toString(e->u, cb);
            cb(e, "()", WalkType::Normal);
            break;

        case RDIL_Goto:
            cb(e, "goto", WalkType::Mnemonic);
            cb(e, " ", WalkType::Whitespace);
            RDIL::toString(e->u, cb);
            break;

        case RDIL_Mem:
            cb(e, "[", WalkType::Normal);
            RDIL::toString(e->u, cb);
            cb(e, "]", WalkType::Normal);
            break;

        case RDIL_Push:
            cb(e, "push", WalkType::Mnemonic);
            cb(e, "(", WalkType::Normal);
            RDIL::toString(e->u, cb);
            cb(e, ")", WalkType::Normal);
            break;

        case RDIL_Pop:
            RDIL::toString(e->u, cb);
            cb(e, "=", WalkType::Normal);
            cb(e, "pop()", WalkType::Mnemonic);
            break;

        case RDIL_Unknown: cb(e, "unknown", WalkType::Mnemonic); break;
        case RDIL_Nop: cb(e, "nop", WalkType::Mnemonic); break;
        default: cb(e, "???", WalkType::Normal); break;
    }
}

void RDIL::walk(const ILExpression* e, const WalkCallback& cb)
{
    if(!e) return;

    switch(e->type)
    {
        case RDIL_Add:
        case RDIL_Sub:
        case RDIL_Mul:
        case RDIL_Div:
        case RDIL_Mod:
        case RDIL_And:
        case RDIL_Or:
        case RDIL_Xor:
        case RDIL_Lsl:
        case RDIL_Lsr:
        case RDIL_Asl:
        case RDIL_Asr:
        case RDIL_Eq:
        case RDIL_Ne:
        case RDIL_Lt:
        case RDIL_Le:
        case RDIL_Gt:
        case RDIL_Ge:
            RDIL::walk(e->left, cb);
            cb(e);
            RDIL::walk(e->right, cb);
            break;

        case RDIL_Rol:
        case RDIL_Ror:
            cb(e);
            RDIL::walk(e->left, cb);
            RDIL::walk(e->right, cb);
            break;

        case RDIL_If:
            cb(e);
            RDIL::walk(e->cond, cb);
            RDIL::walk(e->t, cb);
            RDIL::walk(e->f, cb);
            break;

        case RDIL_Ret:
            cb(e);
            RDIL::walk(e->cond, cb);
            break;

        case RDIL_Cnst:
        case RDIL_Var:
        case RDIL_Reg:
        case RDIL_Unknown:
        case RDIL_Nop:
            cb(e);
            break;

        case RDIL_Copy:
            RDIL::walk(e->dst, cb);
            cb(e);
            RDIL::walk(e->src, cb);
            break;

        case RDIL_Call:
        case RDIL_Pop:
            RDIL::walk(e->u, cb);
            cb(e);
            break;

        case RDIL_Goto:
        case RDIL_Push:
        case RDIL_Mem:
        case RDIL_Not:
            cb(e);
            RDIL::walk(e->u, cb);
            break;

        default:
            REDasmError("Unknown RDIL Type: #" + std::to_string(e->type));
            break;
    }
}

bool RDIL::match(const ILExpression* e, const char* m)
{
    if(!e || !m) return false;

    std::string res;
    if(!RDIL::format(e, res)) return false;

    Lexer l1(m), l2(res.c_str());
    const char* err = nullptr;
    const RDToken* tokens = nullptr;
    size_t c = l1.consume(&tokens, &err);

    if(err)
    {
        rd_cfg->log(err);
        return false;
    }

    RDToken t;
    size_t i = 0;

    while(l2.lex(&t))
    {
        if(i >= c) return false;
        if(!Lexer::equals(&t, &tokens[i])) return false;
        i++;
    }

    return c;
}

bool RDIL::format(const ILExpression* e, std::string& res)
{
    switch(e->type)
    {
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
            RDIL::wrapFormat(e->left, res);
            res += RDIL::textOp(e);
            RDIL::wrapFormat(e->right, res);
            break;

        case RDIL_Not:
            res += RDIL::textOp(e);
            RDIL::wrapFormat(e->u, res);
            break;

        case RDIL_If:
            res += "if(";
            RDIL::format(e->cond, res);
            res += ")";
            RDIL::format(e->t, res);
            res += " else ";
            RDIL::format(e->f, res);
            break;

        case RDIL_Copy:    RDIL::format(e->dst, res); res += " = "; RDIL::format(e->src, res); break;
        case RDIL_Call:    RDIL::format(e->u, res); res += "()"; break;
        case RDIL_Goto:    res += "goto "; RDIL::format(e->u, res); break;
        case RDIL_Mem:     res += "["; RDIL::format(e->u, res); res += "]"; break;
        case RDIL_Push:    res += "push("; RDIL::format(e->u, res); res += ")"; break;
        case RDIL_Pop:     RDIL::format(e->u, res); res += " = pop()"; break;
        case RDIL_Ret:     res += "ret("; RDIL::format(e->cond, res); res += ")"; break;
        case RDIL_Unknown: res += "unknown "; break;
        case RDIL_Nop:     res += "nop "; break;
        case RDIL_Cnst:    res += "cnst"; break;
        case RDIL_Var:     res += "var"; break;
        case RDIL_Reg:     res += "reg"; break;
        default: REDasmError("Unknown IL Expression"); break;
    }

    return true;
}

void RDIL::wrapWalk(const ILExpression* e, const RDIL::ToStringCallback& cb)
{
    if(RDIL::hasValue(e) || IS_TYPE(e, RDIL_Mem))
    {
        RDIL::toString(e, cb);
        return;
    }

    cb(e, "(", WalkType::Normal);
    RDIL::toString(e, cb);
    cb(e, ")", WalkType::Normal);
}

void RDIL::wrapFormat(const ILExpression* e, std::string& res)
{
    if(RDIL::hasValue(e) || IS_TYPE(e, RDIL_Mem))
    {
        RDIL::format(e, res);
        return;
    }

    res += "(";
    RDIL::format(e, res);
    res += ")";
}

void RDIL::getText(const ILExpression* e, std::string& res)
{
    RDIL::toString(e, [&res](const ILExpression* expr, const std::string& s, WalkType) {
        switch(expr->type) {
            case RDIL_Cnst: res += Utils::hex(expr->u_value); break;
            case RDIL_Var: res += expr->var; break;
            case RDIL_Reg: res += expr->reg; break;
            default: res += s; break;
        }
    });
}
