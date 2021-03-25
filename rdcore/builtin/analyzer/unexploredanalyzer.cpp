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

    const rd_address* segments = nullptr;
    size_t c = doc->getSegments(&segments);

    for(size_t i = 0; i < c; i++)
    {
        RDSegment segment;
        if(!doc->addressToSegment(segments[i], &segment) || !HAS_FLAG(&segment, SegmentFlags_Code)) continue;

        const auto* blocks = doc->getBlocks(segment.address);
        if(!blocks) continue;

        for(auto it = blocks->begin(); it != blocks->end(); it++)
        {
            rd_cfg->status("Searching unexplored blocks @ " + Utils::hex(it->address, bits));

            const RDBlock& block = *it;
            if(!IS_TYPE(&block, BlockType_Unknown) || m_done.count(block.address)) continue;
            if(doc->getFlags(block.address) & AddressFlags_Explored) continue;

            m_done.insert(block.address);
            pending.push_back(block);
        }
    }

    if(!pending.empty()) rd_cfg->log("Found " + std::to_string(pending.size()) + " unknown block(s)");

    while(!pending.empty())
    {
        ctx->disassembleBlock(std::addressof(pending.front()));
        pending.pop_front();
    }
}

void UnexploredAnalyzer::clearDone() { m_done.clear(); }
