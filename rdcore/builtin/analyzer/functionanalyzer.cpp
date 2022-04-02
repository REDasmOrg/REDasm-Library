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
    RDLocation entry = document->getEntry();

    const rd_address* addresses = nullptr;
    size_t c = document->getFunctions(&addresses);

    for(size_t i = 0; i < c; i++)
    {
        rd_address address = addresses[i];

        ctx->statusAddress("Analyzing function" , address);
        if(entry.valid && (entry.address == address)) continue; // Don't rename EP, if any
        RDBufferView view;
        if(!document->getView(address, RD_NVAL, &view)) continue;

        std::unique_ptr<ILExpression> e(ILFunction::generateOne(ctx, address));
        if(!e) continue;

        if(FunctionAnalyzer::findNullSubs(ctx, e.get(), address)) continue;
        FunctionAnalyzer::findThunk(ctx, e.get(), address);
    }
}

bool FunctionAnalyzer::findNullSubs(Context* ctx, const ILExpression* expr, rd_address address)
{
    if((expr->type == RDIL_Ret) || ((expr->type == RDIL_Nop) && (ctx->document()->getFunctionInstrCount(address)) == 1))
    {
        ctx->document()->updateLabel(address, "nullsub_" + Utils::hex(address));
        return true;
    }

    return false;
}

std::string FunctionAnalyzer::findThunk(Context* ctx, const ILExpression* expr, rd_address address, int level)
{
    const RDILValue* values = nullptr;
    if(!RDIL::match(expr, "goto [cnst]") && RDIL::match(expr, "goto cnst")) return std::string();

    rd_address raddress = 0;
    std::string label;

    if(size_t n = RDIL::extract(expr, &values); n == 1)
    {
        raddress = values->address;
        auto dlabel = ctx->document()->getLabel(raddress);
        if(dlabel) label = *dlabel;
    }

    if(label.empty()) return std::string();

    std::unique_ptr<ILExpression> rexpr(ILFunction::generateOne(ctx, raddress));

    if(rexpr) // Try to recurse
    {
        std::string newname = FunctionAnalyzer::findThunk(ctx, rexpr.get(), raddress, level + 1);
        if(!newname.empty()) label = newname;
    }

    if(level == 1) ctx->document()->setLabel(address, AddressFlags_None, Utils::thunk(label));
    return label;
}
