#include "rdil.h"
#include "../renderer/renderer.h"
#include "../support/lexer.h"
#include "../config.h"
#include <unordered_map>

#define RDIL_N(x) {x, #x}

const char* RDIL::getOpName(rd_type t)
{
    static const std::unordered_map<rd_type, const char*> NAMES = {
        RDIL_N(RDIL_Unknown), RDIL_N(RDIL_Nop),
        RDIL_N(RDIL_Reg), RDIL_N(RDIL_Cnst), RDIL_N(RDIL_Var),
        RDIL_N(RDIL_Add), RDIL_N(RDIL_Sub), RDIL_N(RDIL_Mul), RDIL_N(RDIL_Div),
        RDIL_N(RDIL_And), RDIL_N(RDIL_Or), RDIL_N(RDIL_Xor), RDIL_N(RDIL_Not),
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
        { "add", RDIL_Add }, { "sub", RDIL_Sub }, { "mul", RDIL_Mul }, { "div", RDIL_Div },
        { "and", RDIL_And }, { "or", RDIL_Or }, { "xor", RDIL_Xor }, { "not", RDIL_Not },
        { "mem", RDIL_Mem }, { "copy", RDIL_Copy },
        { "goto", RDIL_Goto }, { "call", RDIL_Call }, { "ret", RDIL_Ret },
        { "eq", RDIL_Eq }, { "ne", RDIL_Ne },
        { "push", RDIL_Push }, { "pop", RDIL_Pop }
    };

    auto it = IDS.find(id);
    return (it != IDS.end()) ? it->second : static_cast<rd_type>(RD_NPOS);
}

std::string RDIL::getText(const ILExpression* e)
{
    std::string res;
    RDIL::getText(e, res);
    return res;
}

void RDIL::render(const ILExpression* e, const Renderer* renderer, RendererItem* ritem, rd_address address)
{
    RDIL::walk(e, [renderer, ritem, address](const ILExpression* expr, const std::string& s, WalkType wt) {
        if(wt == WalkType::Mnemonic) {
            switch(expr->type) {
                case RDIL_Goto: renderer->renderMnemonic(ritem, s, Theme_Jump); break;
                case RDIL_Ret: renderer->renderMnemonic(ritem, s, Theme_Ret); break;
                case RDIL_Nop: renderer->renderMnemonic(ritem, s, Theme_Nop); break;

                case RDIL_Unknown: {
                    renderer->renderMnemonic(ritem, s, Theme_Default);

                    if(address != RD_NPOS) {
                        renderer->renderText(ritem, " {");
                        renderer->renderAssemblerInstruction(address, ritem);
                        renderer->renderText(ritem, "}");
                    }

                    break;
                }

                default: renderer->renderMnemonic(ritem, s, Theme_Default); break;
            }

            return;
        }

        switch(expr->type) {
            case RDIL_Cnst: renderer->renderUnsigned(ritem, expr->u_value); break;
            case RDIL_Var:  renderer->renderText(ritem, expr->var, Theme_Symbol); break;
            case RDIL_Reg:  renderer->renderRegister(ritem, expr->reg); break;
            default: renderer->renderText(ritem, s); break;
        }
    });
}

const ILExpression* RDIL::extract(const ILExpression* e, const char* q)
{
    RDILQuery query;
    if(!RDIL::parseQuery(q, query)) return nullptr;
    return RDIL::extract(e, { }, query);
}

std::string RDIL::textOp(const ILExpression* e)
{
    switch(e->type)
    {
        case RDIL_Add: return "+";
        case RDIL_Sub: return "-";
        case RDIL_Mul: return "*";
        case RDIL_Div: return "/";
        case RDIL_And: return "&";
        case RDIL_Or:  return "|";
        case RDIL_Xor: return "^";
        case RDIL_Not: return "~";
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

const ILExpression* RDIL::extract(const ILExpression* e, const RDILQueryItem& qi, RDIL::RDILQuery& query, int depth)
{
    if(!e || query.empty()) return e;
    if(depth && (qi.opcode != "*") && (e->type != RDIL::getOpType(qi.opcode))) return nullptr;

    RDILQueryItem nqi = query.front();
    query.pop();

    if(nqi.nodeid == "u") return RDIL::extract(e->u, nqi, query, ++depth);
    if(nqi.nodeid == "cond") return RDIL::extract(e->cond, nqi, query, ++depth);
    if(nqi.nodeid == "dst") return RDIL::extract(e->dst, nqi, query, ++depth);
    if(nqi.nodeid == "left") return RDIL::extract(e->left, nqi, query, ++depth);
    if(nqi.nodeid == "t") return RDIL::extract(e->t, nqi, query, ++depth);
    if(nqi.nodeid == "src") return RDIL::extract(e->src, nqi, query, ++depth);
    if(nqi.nodeid == "right") return RDIL::extract(e->right, nqi, query, ++depth);
    if(nqi.nodeid == "f") return RDIL::extract(e->f, nqi, query, ++depth);
    rd_cfg->log("Unknown node id: " + nqi.nodeid);
    return nullptr;
}

bool RDIL::parseQuery(const std::string& q, RDIL::RDILQuery& query)
{
    Lexer l(q.c_str());
    RDToken token;
    RDILQueryItem item;

    while(!l.hasError() && l.lex(&token))
    {
        if(IS_TYPE(&token, TokenType_Slash)) continue;
        if(!l.check(&token, TokenType_Identifier)) break;
        item.nodeid = Lexer::tokenValue(&token);

        if(!l.lexCheck(&token, TokenType_Colon)) break;
        if(!l.lex(&token)) break;

        switch(token.type)
        {
            case TokenType_Asterisk:
            case TokenType_Identifier: break;

            default: l.error(&token); continue;
        }

        item.opcode = Lexer::tokenValue(&token);
        query.push(item);
    }

    if(l.hasError())
    {
        rd_cfg->log(l.lastError());
        return false;
    }

    return !query.empty();
}

void RDIL::walk(const ILExpression* e, const RDIL::WalkCallback& cb)
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
            RDIL::wrapWalk(e->left, cb);
            cb(e, RDIL::textOp(e), WalkType::Normal);
            RDIL::walk(e->right, cb);
            break;

        case RDIL_Not:
            cb(e, RDIL::textOp(e), WalkType::Normal);
            RDIL::wrapWalk(e->u, cb);
            break;

        case RDIL_If:
            cb(e, "if", WalkType::Mnemonic);
            cb(e, " ", WalkType::Normal);
            cb(e, "(", WalkType::Normal);
            RDIL::walk(e->cond, cb);
            cb(e, ") ", WalkType::Normal);
            RDIL::walk(e->t, cb);
            cb(e, " ", WalkType::Whitespace);
            cb(e, "else", WalkType::Mnemonic);
            cb(e, " ", WalkType::Whitespace);
            RDIL::walk(e->f, cb);
            break;

        case RDIL_Ret:
            cb(e, "ret", WalkType::Mnemonic);
            cb(e, "(", WalkType::Normal);
            RDIL::walk(e->cond, cb);
            cb(e, ")", WalkType::Normal);
            break;

        case RDIL_Cnst:
        case RDIL_Var:
        case RDIL_Reg:
            cb(e, std::string(), WalkType::Normal);
            break;

        case RDIL_Copy:
            RDIL::walk(e->dst, cb);
            cb(e, "=", WalkType::Normal);
            RDIL::walk(e->src, cb);
            break;

        case RDIL_Call:
            RDIL::walk(e->u, cb);
            cb(e, "()", WalkType::Normal);
            break;

        case RDIL_Goto:
            cb(e, "goto", WalkType::Mnemonic);
            cb(e, " ", WalkType::Whitespace);
            RDIL::walk(e->u, cb);
            break;

        case RDIL_Mem:
            cb(e, "[", WalkType::Normal);
            RDIL::walk(e->u, cb);
            cb(e, "]", WalkType::Normal);
            break;

        case RDIL_Push:
            cb(e, "push", WalkType::Mnemonic);
            cb(e, "(", WalkType::Normal);
            RDIL::walk(e->u, cb);
            cb(e, ")", WalkType::Normal);
            break;

        case RDIL_Pop:
            RDIL::walk(e->u, cb);
            cb(e, "=", WalkType::Normal);
            cb(e, "pop()", WalkType::Mnemonic);
            break;

        case RDIL_Unknown: cb(e, "unknown", WalkType::Mnemonic); break;
        case RDIL_Nop: cb(e, "nop", WalkType::Mnemonic); break;
        default: cb(e, "???", WalkType::Normal); break;
    }
}

bool RDIL::match(const ILExpression* e, const char* m)
{
    if(!e || !m) return false;

    std::string res;
    if(!RDIL::match(e, res)) return false;

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

bool RDIL::match(const ILExpression* e, std::string& res)
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
            RDIL::wrapMatch(e->left, res);
            res += RDIL::textOp(e);
            RDIL::wrapMatch(e->right, res);
            break;

        case RDIL_Not:
            res += RDIL::textOp(e);
            RDIL::wrapMatch(e->u, res);
            break;

        case RDIL_If:
            res += "if(";
            RDIL::match(e->cond, res);
            res += ")";
            RDIL::match(e->t, res);
            res += " else ";
            RDIL::match(e->f, res);
            break;

        case RDIL_Copy:    RDIL::match(e->dst, res); res += " = "; RDIL::match(e->src, res); break;
        case RDIL_Call:    RDIL::match(e->u, res); res += "()"; break;
        case RDIL_Goto:    res += "goto "; RDIL::match(e->u, res); break;
        case RDIL_Mem:     res += "["; RDIL::match(e->u, res); res += "]"; break;
        case RDIL_Push:    res += "push("; RDIL::match(e->u, res); res += ")"; break;
        case RDIL_Pop:     RDIL::match(e->u, res); res += " = pop()"; break;
        case RDIL_Ret:     res += "ret("; RDIL::match(e->cond, res); res += ")"; break;
        case RDIL_Unknown: res += "unknown "; break;
        case RDIL_Nop:     res += "nop "; break;
        case RDIL_Cnst:    res += "cnst"; break;
        case RDIL_Var:     res += "var"; break;
        case RDIL_Reg:     res += "reg"; break;
        default: REDasmError("Unknown IL Expression"); break;
    }

    return true;
}

void RDIL::wrapWalk(const ILExpression* e, const RDIL::WalkCallback& cb)
{
    if(RDIL::hasValue(e) || IS_TYPE(e, RDIL_Mem))
    {
        RDIL::walk(e, cb);
        return;
    }

    cb(e, "(", WalkType::Normal);
    RDIL::walk(e, cb);
    cb(e, ")", WalkType::Normal);
}

void RDIL::wrapMatch(const ILExpression* e, std::string& res)
{
    if(RDIL::hasValue(e) || IS_TYPE(e, RDIL_Mem))
    {
        RDIL::match(e, res);
        return;
    }

    res += "(";
    RDIL::match(e, res);
    res += ")";
}

void RDIL::getText(const ILExpression* e, std::string& res)
{
    RDIL::walk(e, [&res](const ILExpression* expr, const std::string& s, WalkType) {
        switch(expr->type) {
            case RDIL_Cnst: res += Utils::hex(expr->u_value); break;
            case RDIL_Var: res += expr->var; break;
            case RDIL_Reg: res += expr->reg; break;
            default: res += s; break;
        }
    });
}
