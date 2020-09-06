#include "stringsanalyzer.h"
#include "../../disassembler.h"
#include "../../engine/stringfinder.h"
#include "../builtin.h"
#include <algorithm>
#include <execution>
#include <deque>

RDAnalyzerPlugin analyzer_Strings = RD_BUILTIN_PLUGIN(analyzerstring_builtin, "Find All Strings", 1,
                                                      "Mark strings in all segments", AnalyzerFlags_RunOnce,
                                                      [](const RDAnalyzerPlugin*, const RDLoader*, const RDAssembler*) -> bool { return true; },
                                                      [](const RDAnalyzerPlugin*, RDDisassembler* d) { StringsAnalyzer::analyze(CPTR(Disassembler, d)); });

void StringsAnalyzer::analyze(Disassembler* disassembler)
{
    std::deque<RDBlock> pendingblocks;
    auto& doc = disassembler->document();

    for(size_t i = 0; i < doc->segmentsCount(); i++)
    {
        RDSegment segment;
        if(!doc->segmentAt(i, &segment) || HAS_FLAG(&segment, SegmentFlags_Bss) || !HAS_FLAG(&segment, SegmentFlags_Data)) continue;

        const auto* blocks = doc->blocks(segment.address);
        if(!blocks) continue;

        for(size_t j = 0; j < blocks->size(); j++)
        {
            const RDBlock& block = blocks->at(j);
            if(!IS_TYPE(&block, BlockType_Unexplored)) continue;
            pendingblocks.push_back(block);
        }
    }

    std::for_each(std::execution::par_unseq, pendingblocks.begin(), pendingblocks.end(), [&](const RDBlock& b) {
        RDBufferView view;
        if(!disassembler->view(b.address, BlockContainer::size(&b), &view)) return;
        StringFinder::find(disassembler, view);
    });
}
