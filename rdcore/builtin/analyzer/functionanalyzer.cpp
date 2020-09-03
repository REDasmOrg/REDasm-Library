#include "functionanalyzer.h"
#include "../../rdil/ilfunction.h"
#include "../../rdil/rdil.h"
#include "../../document/document.h"
#include "../../disassembler.h"
#include "../builtin.h"

RDAnalyzerPlugin analyzer_Function = RD_BUILTIN_PLUGIN(analyzerfunction_builtin, "Discover Functions", static_cast<u64>(-1),
                                                       "Autorename Nullsubs and Thunks", AnalyzerFlags_Selected,
                                                       [](const RDAnalyzerPlugin*, const RDLoaderPlugin*, const RDAssemblerPlugin*) -> bool { return true; },
                                                       [](const RDAnalyzerPlugin*, RDDisassembler* d) { FunctionAnalyzer::analyze(CPTR(Disassembler, d)); });

void FunctionAnalyzer::analyze(Disassembler* disassembler)
{
    auto& document = disassembler->document();
    const RDSymbol* entry = document->entry();

    for(size_t i = 0; i < document->functionsCount(); i++)
    {
        RDLocation loc = document->functionAt(i);
        if(!loc.valid) continue;
        if(entry && (entry->address == loc.address)) continue; // Don't rename EP, if any

        RDBufferView view;
        if(!disassembler->view(loc.address, RD_NPOS, &view)) continue;

        ILFunction il(disassembler);
        if(!ILFunction::generate(loc.address, &il) || (il.size() > 1)) continue;

        if(FunctionAnalyzer::findNullSubs(disassembler, &il, loc.address)) continue;
        FunctionAnalyzer::findThunk(disassembler, &il, loc.address);
    }
}

bool FunctionAnalyzer::findNullSubs(Disassembler* disassembler, const ILFunction* il, rd_address address)
{
    const auto* expr = il->first();

    if((expr->type == RDIL_Ret) || (expr->type == RDIL_Nop))
        return disassembler->document()->rename(address, "nullsub_" + Utils::hex(address));

    return false;
}

void FunctionAnalyzer::findThunk(Disassembler* disassembler, const ILFunction* il, rd_address address)
{
    const auto* expr = il->first();
    const char* name = nullptr;

    if(RDIL::match(expr, "goto [c]")) name = disassembler->document()->name(expr->u->u->address);
    else if(RDIL::match(expr, "goto c")) name = disassembler->document()->name(expr->u->address);
    if(name) disassembler->document()->rename(address, Utils::thunk(name));
}
