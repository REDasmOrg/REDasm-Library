#pragma once

#include <optional>
#include <rdapi/buffer.h>
#include "../../support/safe_ptr.h"
#include "addressqueue.h"

class DocumentNet;
class EmulateResult;

class Algorithm: public AddressQueue
{
    private:
        enum { OK, SKIP, FAIL };

    public:
        Algorithm(Context* ctx);
        void enqueue(rd_address address) override;
        void schedule(rd_address address) override;
        void disassembleBlock(const RDBlock* block);
        void disassemble();

    protected:
        void nextAddress(rd_address address) override;

    private:
        std::optional<rd_address> decode(rd_address address);
        std::optional<rd_address> decode(RDBufferView* view, EmulateResult* result);
        bool isAddressValid(rd_address address) const;
        bool canBeDisassembled(rd_address address) const;
        rd_address processDelaySlots(rd_address address, size_t ds);
        void processResult(EmulateResult* result);
        void processBranches(DocumentNet* net, rd_type forktype, rd_address fromaddress, rd_address address, const RDSegment* segment);

    private:
        mutable RDSegment m_currentsegment{ };
};

typedef safe_ptr<Algorithm> SafeAlgorithm;
