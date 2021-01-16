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

    doc->segments()->each([&](const RDSegment& segment) {
        if(HAS_FLAG(&segment, SegmentFlags_Bss) || !HAS_FLAG(&segment, SegmentFlags_Data)) return true;

        const auto* blocks = doc->blocks(segment.address);
        if(!blocks) return true;

        blocks->each([&](const RDBlock& block) {
            if(IS_TYPE(&block, BlockType_Unknown)) pendingblocks.push_back(block);
            return true;
        });

        return true;
    });

    std::for_each(pendingblocks.begin(), pendingblocks.end(), [&](const RDBlock& b) {
        RDBufferView view;
        if(!doc->view(b.address, BlockContainer::size(&b), &view)) return;
        StringFinder::find(ctx, view);
    });
}
