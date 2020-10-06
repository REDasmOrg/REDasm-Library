#include "unexploredanalyzer.h"
#include "../../document/document.h"
#include "../../engine/stringfinder.h"
#include "../../plugin/assembler.h"
#include "../../support/utils.h"
#include "../../disassembler.h"
#include "../../context.h"
#include "../../config.h"
#include "../builtin.h"
#include <algorithm>
#include <execution>
#include <deque>

RDEntryAnalyzer analyzerEntry_Unexplored = RD_BUILTIN_ENTRY(analyzerunexplored_builtin, "Aggressive Disassembly", 0,
                                                            "Disassemble unknown blocks", AnalyzerFlags_RunOnce | AnalyzerFlags_Experimental,
                                                            [](const RDContext*) -> bool { return true; },
                                                            [](RDContext* ctx) { UnexploredAnalyzer::analyze(CPTR(Context, ctx)); });

void UnexploredAnalyzer::analyze(Context* ctx)
{
    auto& doc = ctx->document();
    size_t bits = ctx->assembler()->bits();

    for(size_t i = 0; i < doc->segmentsCount(); i++)
    {
        RDSegment segment;
        if(!doc->segmentAt(i, &segment) || !HAS_FLAG(&segment, SegmentFlags_Code)) continue;

        const BlockContainer* bc = doc->blocks(segment.address);

        for(size_t j = 0; j < bc->size(); j++)
        {
            const RDBlock& block = bc->at(j);
            rd_cfg->status("Searching unexplored blocks @ " + Utils::hex(block.address, bits));
            if(IS_TYPE(&block, BlockType_Unexplored)) ctx->disassembler()->enqueue(block.address);
        }
    }
}
