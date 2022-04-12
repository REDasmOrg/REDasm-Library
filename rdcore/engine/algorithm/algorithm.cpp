#include "algorithm.h"
#include "../../database/addressdatabase.h"
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
            BufferView::move(&view, 1);
            address++;
        }
        else
        {
            BufferView::move(&view, result.size());
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
        spdlog::info("Algorithm::processingDelaySlots({:x}, {}): {}", address, ds, i);

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
            case EmulateResult::RefData: m_document->checkData(result->address(), res.address, res.size); break;
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
            spdlog::info("Algorithm::processBranches(): TRUE @ {:x} (from {:x})", v.address, fromaddress);
            m_net->linkBranch(fromaddress, v.address, forktype);
            break;

        case EmulateResult::BranchFalse:
            spdlog::info("Algorithm::processBranches(): FALSE @ {:x} (from {:x})", v.address, fromaddress);
            m_net->linkBranch(fromaddress, v.address, forktype);
            this->schedule(v.address);
            return; // Don't generate symbols

        case EmulateResult::BranchTable:
            spdlog::info("Algorithm::processBranches(): TABLE @ {:x} (from {:x})", v.address, fromaddress);
            this->processBranchTable(fromaddress, v);
            return;

        default: return;
    }

    if(HAS_FLAG(segment, SegmentFlags_Code))
    {
        int dir = Utils::branchDirection(fromaddress, v.address);
        m_document->setBranch(v.address, dir);
        if(dir) this->schedule(v.address);
        return;
    }

    m_document->setLocation(v.address);
}

void Algorithm::processCalls(rd_type forktype, rd_address fromaddress, const EmulateResult::Value& v, const RDSegment* segment)
{
    switch(forktype)
    {
        case EmulateResult::CallTable:
            spdlog::info("Algorithm::processCalls(): TABLE @ {:x} (from {:x})", v.address, fromaddress);
            this->processCallTable(fromaddress, v);
            break;

        case EmulateResult::Call: {
            spdlog::info("Algorithm::processCalls(): CALL @ {:x} (from {:x})", v.address, fromaddress);

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

    WeakScope weak(this->context());
    Assembler* assembler = this->context()->getAssembler(result->address());

    if(!assembler)
    {
        spdlog::error("Algorithm::decode(): Assembler not found @ {:x}", result->address());
        return std::nullopt;
    }

    this->status("Decoding @ " + Utils::hex(result->address()));
    spdlog::trace("Algorithm::decode(): {:x} as '{}'", result->address(), assembler->id());
    assembler->emulate(result);

    if(!result->size())
    {
        spdlog::trace("Algorithm::decode(): Invalid instruction @ {:x} (Size is empty)", result->address());
        return std::nullopt;
    }

    if(result->size() > view->size)
    {
        spdlog::trace("Algorithm::decode(): Invalid instruction @ {:x} (Block too big)", result->address());
        return std::nullopt;
    }

    if(!result->invalid())
        m_document->setCode(result->address(), result->size(), this->addressDatabase()->assemblerToIndex(assembler->id()));

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
    else
        spdlog::warn("Algorithm::decode(): STOP @ {:x}", result->address());

    return std::nullopt;
}

bool Algorithm::isAddressValid(rd_address address) const
{
    RDSegment segment;

    if(!m_document->addressToSegment(address, &segment))
    {
        spdlog::warn("Algorithm::isAddressValid({:x}): Invalid segment, skipping...", address);
        return false;
    }


    if(!HAS_FLAG(&segment, SegmentFlags_Code))
    {
        spdlog::warn("Algorithm::isAddressValid({:x}): Segment does not contains code, skipping...", address);
        return false;
    }

    RDBlock block;

    if(!m_document->addressToBlock(address, &block))
    {
        spdlog::warn("Algorithm::isAddressValid({:x}): Invalid block, skipping...", address);
        return false;
    }

    if(!IS_TYPE(&block, BlockType_Unknown))
    {
        rd_flag flags = m_document->getFlags(block.address);

        if(IS_TYPE(&block, BlockType_Data) && !this->context()->isWeak())
        {
            if((flags & AddressFlags_AsciiString) || (flags & AddressFlags_WideString))
            {
                spdlog::warn("Algorithm::isAddressValid({:x}): Block is string, skipping...", address, block.type);
                return false;
            }

            spdlog::warn("Algorithm::isAddressValid({:x}): Overriding Data block...", address, block.type);
            return true;
        }

        if(!IS_TYPE(&block, BlockType_Data) && m_document->isWeak(block.address))
        {
            if((flags & AddressFlags_AsciiString) || (flags & AddressFlags_WideString))
            {
                spdlog::warn("Algorithm::isAddressValid({:x}): Block is string, skipping...", address, block.type);
                return false;
            }

            spdlog::warn("Algorithm::isAddressValid({:x}): Block is weak, overriding...", address, block.type);
            return true;
        }

        spdlog::warn("Algorithm::isAddressValid({:x}): Block type is #{}, skipping...", address, block.type);
        return false;
    }

    return true;
}
