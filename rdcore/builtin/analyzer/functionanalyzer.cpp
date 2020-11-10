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
    Loader* loader = ctx->loader();

    const FunctionContainer* functions = document->functions();

    functions->each([&](rd_address address) {
        ctx->statusAddress("Analyzing function" , address);
        if(entry && (entry->address == address)) return true; // Don't rename EP, if any
        RDBufferView view;
        if(!loader->view(address, RD_NVAL, &view)) return true;

        ILFunction il(ctx);
        if(!ILFunction::generate(address, &il) || (il.size() > 1)) return true;

        if(FunctionAnalyzer::findNullSubs(ctx, &il, address)) return true;
        FunctionAnalyzer::findThunk(ctx, &il, address);
        return true;
    });
}

bool FunctionAnalyzer::findNullSubs(Context* ctx, const ILFunction* il, rd_address address)
{
    const auto* expr = il->first();

    if((expr->type == RDIL_Ret) || (expr->type == RDIL_Nop))
        return ctx->document()->rename(address, "nullsub_" + Utils::hex(address));

    return false;
}

void FunctionAnalyzer::findThunk(Context* ctx, const ILFunction* il, rd_address address)
{
    const auto* expr = il->first();
    const char* name = nullptr;

    if(RDIL::match(expr, "goto [cnst]")) name = ctx->document()->name(RDIL::extract(expr, "u:mem/u:cnst")->address);
    else if(RDIL::match(expr, "goto cnst")) name = ctx->document()->name(RDIL::extract(expr, "u:cnst")->address);
    if(name) ctx->document()->rename(address, Utils::thunk(name));
}
