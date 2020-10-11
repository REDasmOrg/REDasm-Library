#include "functionanalyzer.h"
#include "../../rdil/ilfunction.h"
#include "../../rdil/rdil.h"
#include "../../document/document.h"
#include "../../disassembler.h"
#include "../../context.h"
#include "../builtin.h"

RDEntryAnalyzer analyzerEntry_Function = RD_BUILTIN_ENTRY(analyzerfunction_builtin, "Discover Functions", static_cast<u64>(-1),
                                                          "Autorename Nullsubs and Thunks", AnalyzerFlags_Selected,
                                                          [](const RDContext*) -> bool { return true; },
                                                          [](RDContext* ctx) { FunctionAnalyzer::analyze(CPTR(Context, ctx)); });

void FunctionAnalyzer::analyze(Context* ctx)
{
    auto& document = ctx->document();
    const RDSymbol* entry = document->entry();
    Loader* loader = ctx->loader();

    for(size_t i = 0; i < document->functionsCount(); i++)
    {
        RDLocation loc = document->functionAt(i);
        if(!loc.valid) continue;
        if(entry && (entry->address == loc.address)) continue; // Don't rename EP, if any

        RDBufferView view;
        if(!loader->view(loc.address, RD_NPOS, &view)) continue;

        ILFunction il(ctx);
        if(!ILFunction::generate(loc.address, &il) || (il.size() > 1)) continue;

        if(FunctionAnalyzer::findNullSubs(ctx, &il, loc.address)) continue;
        FunctionAnalyzer::findThunk(ctx, &il, loc.address);
    }
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
