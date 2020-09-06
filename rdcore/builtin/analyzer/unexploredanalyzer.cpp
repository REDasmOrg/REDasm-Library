#include "unexploredanalyzer.h"
#include "../../disassembler.h"
#include "../../engine/stringfinder.h"
#include "../../context.h"
#include "../builtin.h"
#include <algorithm>
#include <execution>
#include <deque>

RDAnalyzerPlugin analyzer_Unexplored = RD_BUILTIN_PLUGIN(analyzerunexplored_builtin, "Aggressive Disassembly", 0,
                                                         "Disassemble unknown blocks", AnalyzerFlags_RunOnce | AnalyzerFlags_Experimental,
                                                         [](const RDAnalyzerPlugin*, const RDLoader*, const RDAssembler*) -> bool { return true; },
                                                         [](const RDAnalyzerPlugin*, RDDisassembler* d) { UnexploredAnalyzer::analyze(CPTR(Disassembler, d)); });

void UnexploredAnalyzer::analyze(Disassembler* disassembler)
{
    auto& doc = disassembler->document();
    size_t bits = disassembler->assembler()->bits();

    for(size_t i = 0; i < doc->segmentsCount(); i++)
    {
        RDSegment segment;
        if(!doc->segmentAt(i, &segment) || !HAS_FLAG(&segment, SegmentFlags_Code)) continue;

        const BlockContainer* bc = doc->blocks(segment.address);

        for(size_t j = 0; j < bc->size(); j++)
        {
            const RDBlock& block = bc->at(j);
            rd_ctx->status("Searching unexplored blocks @ " + Utils::hex(block.address, bits));
            if(IS_TYPE(&block, BlockType_Unexplored)) disassembler->enqueue(block.address);
        }
    }
}
