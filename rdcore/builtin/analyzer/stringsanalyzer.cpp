#include "stringsanalyzer.h"
#include "../../document/document.h"
#include "../../disassembler.h"
#include "../../context.h"
#include "../../engine/stringfinder.h"
#include "../builtin.h"
#include <algorithm>
#include <deque>

RDEntryAnalyzer analyzerEntry_Strings = RD_BUILTIN_ENTRY(analyzerstring_builtin, "Find All Strings", std::numeric_limits<u32>::max() - 1,
                                                         "Mark strings in all segments", AnalyzerFlags_RunOnce,
                                                         [](const RDContext*) -> bool { return true; },
                                                         [](RDContext* ctx) { StringsAnalyzer::analyze(CPTR(Context, ctx)); });

void StringsAnalyzer::analyze(Context* ctx)
{
    std::deque<RDBlock> pendingblocks;
    auto& doc = ctx->document();

    const rd_address* segments = nullptr;
    size_t c = doc->getSegments(&segments);

    for(size_t i = 0; i < c; i++)
    {
        RDSegment segment;
        if(!doc->addressToSegment(segments[i], &segment) || (HAS_FLAG(&segment, SegmentFlags_Bss) || !HAS_FLAG(&segment, SegmentFlags_Data))) continue;

        const auto* blocks = doc->getBlocks(segment.address);
        if(!blocks) continue;

        for(auto it = blocks->begin(); it != blocks->end(); it++)
        {
            if(IS_TYPE(std::addressof(*it), BlockType_Unknown))
                pendingblocks.push_back(*it);
        }
    }

    std::for_each(pendingblocks.begin(), pendingblocks.end(), [&](const RDBlock& b) {
        RDBufferView view;
        if(!doc->getView(b.address, BlockContainer::size(&b), &view)) return;
        StringFinder::find(ctx, view);
    });
}
