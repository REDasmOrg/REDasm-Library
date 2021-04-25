#include "algorithm.h"
#include "../../document/document.h"
#include "../../support/error.h"
#include "../../support/utils.h"
#include "../../disassembler.h"
#include "../../context.h"
#include <thread>

Algorithm::Algorithm(Context* ctx): Object(ctx), m_document(ctx->document()), m_net(ctx->net()) { }
bool Algorithm::hasNext() const { return !m_pending.empty(); }
void Algorithm::enqueue(rd_address address) { if(this->isAddressValid(address)) m_pending.push_front(address); }
void Algorithm::schedule(rd_address address) { if(this->isAddressValid(address)) m_pending.push_back(address); }

void Algorithm::disassembleBlock(const RDBlock* block)
{
    if(!block) return;

    RDBufferView view;
    if(!m_document->getBlockView(block->address, &view)) return;

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

void Algorithm::next()
{
    if(m_pending.empty()) return;

    rd_address address = m_pending.front();
    m_pending.pop_front();
    this->nextAddress(address);
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
        if(!m_document->addressToSegment(res.address, &segment)) continue;

        switch(forktype)
        {
            case EmulateResult::Ref: m_document->checkLocation(result->address(), res.address, res.size); break;
            case EmulateResult::RefString: m_document->checkString(result->address(), res.address, res.size); break;
            case EmulateResult::RefTypeName: m_document->checkTypeName(result->address(), res.address, res.name); break;
            case EmulateResult::RefType: m_document->checkType(result->address(), res.address, res.type.get()); break;
            case EmulateResult::Table: this->processTable(result->address(), res); break;

            case EmulateResult::Branch:
            case EmulateResult::BranchTrue:
            case EmulateResult::BranchFalse:
            case EmulateResult::BranchTable:
                this->processBranches(forktype, result->address(), res, &segment);
                break;

            case EmulateResult::Call:
            case EmulateResult::CallTable:
                this->processCalls(forktype, result->address(), res, &segment);
                break;

            default: break;
        }
    }
}

void Algorithm::processBranches(rd_type forktype, rd_address fromaddress, const EmulateResult::Value& v, const RDSegment* segment)
{
    switch(forktype)
    {
        case EmulateResult::Branch:
        case EmulateResult::BranchTrue:
            m_net->linkBranch(fromaddress, v.address, forktype);
            break;

        case EmulateResult::BranchFalse:
            m_net->linkBranch(fromaddress, v.address, forktype);
            this->schedule(v.address);
            return; // Don't generate symbols

        case EmulateResult::BranchTable:
            this->processBranchTable(fromaddress, v);
            return;

        default: return;
    }

    if(HAS_FLAG(segment, SegmentFlags_Code))
    {
        int dir = Utils::branchDirection(fromaddress, v.address);
        //FIXME: if(!dir) m_document->autoComment(fromaddress, "Infinite loop");
        m_document->setBranch(v.address, dir);
        this->schedule(v.address);
        return;
    }

    m_document->setLocation(v.address);
}

void Algorithm::processCalls(rd_type forktype, rd_address fromaddress, const EmulateResult::Value& v, const RDSegment* segment)
{
    switch(forktype)
    {
        case EmulateResult::CallTable: this->processCallTable(fromaddress, v); break;

        case EmulateResult::Call: {
            if(HAS_FLAG(segment, SegmentFlags_Code)) {
                m_net->linkCall(fromaddress, v.address, forktype);
                m_document->setFunction(v.address, std::string());
                this->schedule(v.address);
            }
            else if(rd_address loc = m_document->checkLocation(fromaddress, v.address); loc != RD_NVAL)
                m_net->linkCall(fromaddress, loc, EmulateResult::CallIndirect);

            break;
        }

        default: break;
    }
}

void Algorithm::processBranchTable(rd_address fromaddress, const EmulateResult::Value& v)
{
    RDSegment segment;

    size_t c = m_document->checkTable(fromaddress, v.address, v.size, [&](rd_address, rd_address address, size_t) {
        if(!m_document->addressToSegment(address, &segment) || !HAS_FLAG(&segment, SegmentFlags_Code)) return false;

        m_net->linkBranch(fromaddress, address, EmulateResult::BranchIndirect);
        m_document->setBranch(address);
        this->schedule(address);
        return true;
    });

    if(!c) m_document->checkLocation(fromaddress, v.address);
}

void Algorithm::processCallTable(rd_address fromaddress, const EmulateResult::Value& v)
{
    RDSegment segment;

    size_t c = m_document->checkTable(fromaddress, v.address, v.size, [&](rd_address, rd_address address, size_t) {
        if(!m_document->addressToSegment(address, &segment) || !HAS_FLAG(&segment, SegmentFlags_Code)) return false;

        m_net->linkCall(fromaddress, address, EmulateResult::CallIndirect);
        m_document->setFunction(address, std::string());
        this->schedule(address);
        return true;
    });

    if(!c) m_document->checkLocation(fromaddress, v.address);
}

void Algorithm::processTable(rd_address fromaddress, const EmulateResult::Value& v)
{
    if(m_document->checkPointer(fromaddress, v.address, v.size, nullptr)) return;
    m_document->checkLocation(fromaddress, v.address, v.size);
}

void Algorithm::nextAddress(rd_address address)
{
    auto nextaddress = this->decode(address);
    if(nextaddress) this->enqueue(*nextaddress);
}

std::optional<rd_address> Algorithm::decode(rd_address address)
{
    RDBufferView view;
    if(!m_document->getView(address, RD_NVAL, &view)) return std::nullopt;

    EmulateResult result(address, &view);
    return this->decode(&view, &result);
}

std::optional<rd_address> Algorithm::decode(RDBufferView* view, EmulateResult* result)
{
    if(!this->isAddressValid(result->address())) return std::nullopt;

    this->status("Decoding @ " + Utils::hex(result->address()));
    this->context()->assembler()->emulate(result);

    if(!result->size() || (result->size() > view->size))
        return std::nullopt;

    if(!result->invalid()) m_document->setCode(result->address(), result->size());
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
    RDSegment segment;
    if(!m_document->addressToSegment(address, &segment) || !HAS_FLAG(&segment, SegmentFlags_Code)) return false;

    RDBlock block;
    if(!m_document->addressToBlock(address, &block) || IS_TYPE(&block, BlockType_Code)) return false;
    return true;
}
