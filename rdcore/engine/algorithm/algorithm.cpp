#include "algorithm.h"
#include "../../document/document.h"
#include "../../support/error.h"
#include "../../support/utils.h"
#include "../../disassembler.h"
#include "../../context.h"
#include "emulateresult.h"
#include <thread>

Algorithm::Algorithm(Context* ctx): AddressQueue(ctx) { }
void Algorithm::enqueue(rd_address address) { if(this->isAddressValid(address)) AddressQueue::enqueue(address); }
void Algorithm::schedule(rd_address address) { if(this->isAddressValid(address)) AddressQueue::schedule(address); }

void Algorithm::disassembleBlock(const RDBlock* block)
{
    if(!block) return;

    RDBufferView view;
    if(!m_document->view(*block, &view)) return;

    rd_address address = block->address;

    while(!BufferView::empty(&view))
    {
        EmulateResult result(address, &view);
        auto nextaddress = this->decode(&view, &result);

        if(!nextaddress || !result.size())
        {
            BufferView::advance(&view, 1);
            address++;
        }
        else
        {
            BufferView::advance(&view, result.size());
            address = *nextaddress;
        }

        std::this_thread::yield();
    }

    this->disassemble(); // Check for pending addresses
}

void Algorithm::disassemble()
{
    while(this->hasNext())
    {
        this->next();
        std::this_thread::yield();
    }
}

bool Algorithm::canBeDisassembled(rd_address address, RDBlock* block) const
{
    if(!this->isAddressValid(address)) return false;

    if(!m_document->block(address, block) || IS_TYPE(block, BlockType_Code)) return false;

    if(IS_TYPE(block, BlockType_Data))
    {
        RDSymbol symbol;

        if(!m_document->symbol(block->address, &symbol))
            REDasmError("Invalid symbol", block->address);

        switch(symbol.type)
        {
            case SymbolType_Label:
            case SymbolType_Function: return true;
            default: break;
        }

        if(m_net->getReferences(block->address, nullptr)) return false;
        return HAS_FLAG(&symbol, SymbolFlags_Weak);
    }

    return true;
}

rd_address Algorithm::processDelaySlots(rd_address address, size_t ds)
{
    for(size_t i = ds; i > 0; i--)
    {
        auto nextaddress = this->decode(address);
        if(!nextaddress) break;

        if(i > 1) m_net->linkNext(address, *nextaddress);
        address = *nextaddress;
    }

    return address;
}

void Algorithm::processResult(EmulateResult* result)
{
    for(const auto& [forktype, res] : result->results())
    {
        RDSegment segment;
        if(!m_document->segment(res.address, &segment)) continue;

        switch(forktype)
        {
            case EmulateResult::Ref:
                m_document->checkLocation(result->address(), res.address, res.size);
                break;

            case EmulateResult::Branch:
            case EmulateResult::BranchTrue:
            case EmulateResult::BranchFalse:
                this->processBranches(forktype, result->address(), res.address, &segment);
                break;

            case EmulateResult::Call: {
                m_net->linkCall(result->address(), res.address);

                if(HAS_FLAG(&segment, SegmentFlags_Code)) {
                    m_document->function(res.address, std::string());
                    this->schedule(res.address);
                }
                else m_document->label(res.address);

                break;
            }

            default: break;
        }
    }
}

void Algorithm::processBranches(rd_type forktype, rd_address fromaddress, rd_address address, const RDSegment* segment)
{
    switch(forktype)
    {
        case EmulateResult::Branch:
        case EmulateResult::BranchTrue:
            m_net->linkBranch(fromaddress, address, forktype);
            break;

        case EmulateResult::BranchFalse:
            m_net->linkBranch(fromaddress, address, forktype);
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

    m_document->data(address, this->context()->addressWidth(), std::string());
}

void Algorithm::nextAddress(rd_address address)
{
    auto nextaddress = this->decode(address);
    if(nextaddress) this->enqueue(*nextaddress);
}

std::optional<rd_address> Algorithm::decode(rd_address address)
{
    RDBufferView view;
    if(!m_document->blockView(address, &view)) return std::nullopt;

    EmulateResult result(address, &view);
    return this->decode(&view, &result);
}

std::optional<rd_address> Algorithm::decode(RDBufferView* view, EmulateResult* result)
{
    RDBlock block;
    if(!this->canBeDisassembled(result->address(), &block)) return std::nullopt;
    if(!IS_TYPE(&block, BlockType_Unknown)) m_document->unknown(result->address(), view->size); // Demote and merge the entire block

    this->status("Decoding @ " + Utils::hex(result->address()));
    this->context()->assembler()->emulate(result);

    if(!result->size() || (result->size() > view->size))
    {
        m_document->explored(result->address(), 1);
        return std::nullopt;
    }

    if(!result->invalid()) m_document->instruction(result->address(), result->size());
    rd_address nextaddress = result->address() + result->size();

    if(result->delaySlot())
    {
        m_net->linkNext(result->address(), nextaddress);
        nextaddress = this->processDelaySlots(nextaddress, result->delaySlot());
    }

    this->processResult(result);

    if(result->canFlow())
    {
        m_net->linkNext(result->address(), nextaddress);
        return nextaddress;
    }

    return std::nullopt;
}

bool Algorithm::isAddressValid(rd_address address) const
{
    if(m_document->items()->contains(RDDocumentItem{ address, DocumentItemType_Instruction, 0 })) return false;
    if(!m_document->segment(address, &m_currentsegment)|| !HAS_FLAG(&m_currentsegment, SegmentFlags_Code)) return false;
    return true;
}
