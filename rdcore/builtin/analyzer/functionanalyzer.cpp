#include "functionanalyzer.h"
#include "../../rdil/ilfunction.h"
#include "../../rdil/rdil.h"
#include "../../document/document.h"
#include "../../disassembler.h"
#include "../../context.h"
#include "../builtin.h"

RDEntryAnalyzer analyzerEntry_Function = RD_BUILTIN_ENTRY(analyzerfunction_builtin, "Discover Functions", 0,
                                                          "Autorename Nullsubs and Thunks", AnalyzerFlags_Selected,
                                                          [](const RDContext*) -> bool { return true; },
                                                          [](RDContext* ctx) { FunctionAnalyzer::analyze(CPTR(Context, ctx)); });

void FunctionAnalyzer::analyze(Context* ctx)
{
    auto& document = ctx->document();
    const RDSymbol* entry = document->entry();
    const FunctionContainer& functions = document->functions();

    for(rd_address address : functions)
    {
        ctx->statusAddress("Analyzing function" , address);
        if(entry && (entry->address == address)) continue; // Don't rename EP, if any
        RDBufferView view;
        if(!document->view(address, RD_NVAL, &view)) continue;

        std::unique_ptr<ILExpression> e(ILFunction::generateOne(ctx, address));
        if(!e) continue;

        if(FunctionAnalyzer::findNullSubs(ctx, e.get(), address)) continue;
        FunctionAnalyzer::findThunk(ctx, e.get(), address);
    }
}

bool FunctionAnalyzer::findNullSubs(Context* ctx, const ILExpression* expr, rd_address address)
{
    if((expr->type == RDIL_Ret) || (expr->type == RDIL_Nop && (ctx->document()->getFunctionInstrCount(address)) == 1))
        return ctx->document()->rename(address, "nullsub_" + Utils::hex(address));

    return false;
}

std::string FunctionAnalyzer::findThunk(Context* ctx, const ILExpression* expr, rd_address address, int level)
{
    std::string name;
    rd_address raddress = 0;

    if(RDIL::match(expr, "goto [cnst]"))
    {
        raddress = RDIL::extract(expr, "u:mem/u:cnst")->address;
        const char* pname = ctx->document()->name(raddress);
        if(pname) name = pname;
    }
    else if(RDIL::match(expr, "goto cnst"))
    {
        raddress = RDIL::extract(expr, "u:cnst")->address;
        const char* pname = ctx->document()->name(raddress);
        if(pname) name = pname;
    }

    if(name.empty()) return std::string();

    std::unique_ptr<ILExpression> rexpr(ILFunction::generateOne(ctx, raddress));

    if(rexpr) // Try to recurse
    {
        std::string newname = FunctionAnalyzer::findThunk(ctx, rexpr.get(), raddress, level + 1);
        if(!newname.empty()) name = newname;
    }

    if(level == 1) ctx->document()->rename(address, Utils::thunk(name));
    return name;
}
