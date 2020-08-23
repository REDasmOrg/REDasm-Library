#include "algorithm.h"
#include "../../document/document.h"
#include "../../support/error.h"
#include "../../support/utils.h"
#include "../../disassembler.h"
#include "../../context.h"
#include "emulateresult.h"
#include <rdapi/disassembler.h>

Algorithm::Algorithm(Disassembler* disassembler): AddressQueue(disassembler) { }
void Algorithm::enqueue(rd_address address) { if(this->isAddressValid(address)) AddressQueue::enqueue(address); }
void Algorithm::schedule(rd_address address) { if(this->isAddressValid(address)) AddressQueue::schedule(address); }

bool Algorithm::canBeDisassembled(rd_address address) const
{
    if(!this->isAddressValid(address)) return false;

    RDBlock block;

    if(!m_document->block(address, &block) || IS_TYPE(&block, BlockType_Code)) return false;

    if(IS_TYPE(&block, BlockType_Data))
    {
        RDSymbol symbol;

        if(!m_document->symbol(block.address, &symbol))
            REDasmError("Invalid symbol", block.address);

        switch(symbol.type)
        {
            case SymbolType_Label:
            case SymbolType_Function: return true;

            default: break;
        }

        auto* net = m_disassembler->net();
        if(net->getReferences(block.address, nullptr)) return false;
        return HAS_FLAG(&symbol, SymbolFlags_Weak);
    }

    return true;
}

rd_address Algorithm::processDelaySlots(rd_address address, size_t ds)
{
    for(size_t i = ds; i > 0; i--)
    {
        auto nextaddress = this->decodeAddress(address);
        if(!nextaddress) break;

        if(i > 1) m_disassembler->net()->linkNext(address, *nextaddress);
        address = *nextaddress;
    }

    return address;
}

void Algorithm::processResult(EmulateResult* result)
{
    DocumentNet* net = m_disassembler->net();

    for(const auto& [forktype, res] : result->results())
    {
        RDSegment segment;
        if(!m_document->segment(res.address, &segment)) continue;

        switch(forktype)
        {
            case EmulateResult::Ref:
                m_disassembler->markLocation(result->address(), res.address);
                break;

            case EmulateResult::Branch:
            case EmulateResult::BranchTrue:
            case EmulateResult::BranchFalse:
                this->processBranches(net, forktype, result->address(), res.address, &segment);
                break;

            case EmulateResult::Call:
                net->linkCall(result->address(), res.address);

                if(HAS_FLAG(&segment, SegmentFlags_Code)) {
                    m_document->function(res.address, std::string());
                    this->schedule(res.address);
                }
                else m_document->label(res.address);

                break;

            default: break;
        }
    }
}

void Algorithm::processBranches(DocumentNet* net, rd_type forktype, rd_address fromaddress, rd_address address, const RDSegment* segment)
{
    switch(forktype)
    {
        case EmulateResult::Branch:
        case EmulateResult::BranchTrue:
            net->linkBranch(fromaddress, address, forktype);
            break;

        case EmulateResult::BranchFalse:
            net->linkBranch(fromaddress, address, forktype);
            this->schedule(address);
            return; // Don't generate symbols

        default: return;
    }

    if(HAS_FLAG(segment, SegmentFlags_Code))
    {
        int dir = Utils::branchDirection(fromaddress, address);
        if(!dir) m_document->autoComment(fromaddress, "Infinite loop");
        m_document->branch(address, dir);
        this->schedule(address);
        return;
    }

    m_document->data(address, m_disassembler->assembler()->addressWidth(), std::string());
}

void Algorithm::nextAddress(rd_address address)
{
    auto nextaddress = this->decodeAddress(address);
    if(!nextaddress) return;

    m_disassembler->net()->linkNext(address, *nextaddress);
    this->enqueue(*nextaddress);
}

std::optional<rd_address> Algorithm::decodeAddress(rd_address address)
{
    rd_ctx->status("Decoding @ " + Utils::hex(address));
    if(!this->canBeDisassembled(address)) return std::nullopt;

    RDBufferView view;
    if(!m_disassembler->view(address, SegmentContainer::offsetSize(m_currentsegment), &view)) return std::nullopt;

    EmulateResult result(address, &view, m_disassembler);
    m_disassembler->assembler()->emulate(&result);
    if(!result.size() || (result.size() > view.size)) return std::nullopt;

    m_disassembler->document()->instruction(address, result.size());
    rd_address nextaddress = address + result.size();

    if(result.delaySlot())
    {
        m_disassembler->net()->linkNext(address, nextaddress);
        nextaddress = this->processDelaySlots(nextaddress, result.delaySlot());
    }

    this->processResult(&result);
    return result.canFlow() ? std::make_optional(nextaddress) : std::nullopt;
}

bool Algorithm::isAddressValid(rd_address address) const
{
    auto loc = m_disassembler->loader()->offset(address);
    if(!loc.valid) return false;

    if(m_document->instructionItem(address, nullptr)) return false;
    if(!m_document->segment(address, &m_currentsegment)|| !HAS_FLAG(&m_currentsegment, SegmentFlags_Code)) return false;
    return true;
}
