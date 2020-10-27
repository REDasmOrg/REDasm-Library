#include "stringsanalyzer.h"
#include "../../document/document.h"
#include "../../disassembler.h"
#include "../../context.h"
#include "../../engine/stringfinder.h"
#include "../builtin.h"
#include <algorithm>
#include <execution>
#include <deque>

RDEntryAnalyzer analyzerEntry_Strings = RD_BUILTIN_ENTRY(analyzerstring_builtin, "Find All Strings", 1,
                                                         "Mark strings in all segments", AnalyzerFlags_RunOnce,
                                                         [](const RDContext*) -> bool { return true; },
                                                         [](RDContext* ctx) { StringsAnalyzer::analyze(CPTR(Context, ctx)); });

void StringsAnalyzer::analyze(Context* ctx)
{
    std::deque<RDBlock> pendingblocks;
    auto& doc = ctx->document();

    for(size_t i = 0; i < doc->segmentsCount(); i++)
    {
        RDSegment segment;
        if(!doc->segmentAt(i, &segment) || HAS_FLAG(&segment, SegmentFlags_Bss) || !HAS_FLAG(&segment, SegmentFlags_Data)) continue;

        const auto* blocks = doc->blocks(segment.address);
        if(!blocks) continue;

        blocks->each([&](const RDBlock& block) {
            if(IS_TYPE(&block, BlockType_Unexplored)) pendingblocks.push_back(block);
            return true;
        });
    }

    Loader* loader = ctx->loader();

    std::for_each(std::execution::par_unseq, pendingblocks.begin(), pendingblocks.end(), [&](const RDBlock& b) {
        RDBufferView view;
        if(!loader->view(b.address, BlockContainer::size(&b), &view)) return;
        StringFinder::find(ctx, view);
    });
}
