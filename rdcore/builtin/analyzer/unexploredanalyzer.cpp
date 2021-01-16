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
#include <deque>

RDEntryAnalyzer analyzerEntry_Unexplored = RD_BUILTIN_ENTRY(analyzerunexplored_builtin, "Unexplored Blocks", std::numeric_limits<u32>::max(),
                                                            "Disassemble unexplored blocks", AnalyzerFlags_Experimental | AnalyzerFlags_Selected,
                                                            [](const RDContext*) -> bool { UnexploredAnalyzer::clearDone(); return true; },
                                                            [](RDContext* ctx) { UnexploredAnalyzer::analyze(CPTR(Context, ctx)); });

std::unordered_set<rd_address> UnexploredAnalyzer::m_done;

void UnexploredAnalyzer::analyze(Context* ctx)
{
    auto& doc = ctx->document();
    size_t bits = ctx->assembler()->bits();
    std::deque<RDBlock> pending;
    int c = 0;

    doc->segments()->each([&](const RDSegment& segment) {
        if(!HAS_FLAG(&segment, SegmentFlags_Code)) return true;
        const BlockContainer* blocks = doc->blocks(segment.address);

        blocks->each([&](const RDBlock& block) {
            rd_cfg->status("Searching unexplored blocks @ " + Utils::hex(block.address, bits));
            if(!IS_TYPE(&block, BlockType_Unknown) || HAS_FLAG(&block, BlockFlags_Explored) || m_done.count(block.address)) return true;

            c++;
            m_done.insert(block.address);
            pending.push_back(block);
            return true;
        });

        return true;
    });

    if(c) rd_cfg->log("Found " + std::to_string(c) + " unknown block(s)");

    while(!pending.empty())
    {
        ctx->disassembleBlock(std::addressof(pending.front()));
        pending.pop_front();
    }
}

void UnexploredAnalyzer::clearDone() { m_done.clear(); }
