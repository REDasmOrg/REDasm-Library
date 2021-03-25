#pragma once

#include <optional>
#include <rdapi/buffer.h>
#include "../../support/safe_ptr.h"
#include "emulateresult.h"
#include "addressqueue.h"

class DocumentNet;
class EmulateResult;

class Algorithm: public AddressQueue
{
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
        void processResult(EmulateResult* result);
        void processBranches(rd_type forktype, rd_address fromaddress, const EmulateResult::Value& v, const RDSegment* segment);
        void processCalls(rd_type forktype, rd_address fromaddress, const EmulateResult::Value& v, const RDSegment* segment);
        void processBranchTable(rd_address fromaddress, const EmulateResult::Value& v);
        void processCallTable(rd_address fromaddress, const EmulateResult::Value& v);
        void processTable(rd_address fromaddress, const EmulateResult::Value& v);
        rd_address processDelaySlots(rd_address address, size_t ds);
};

typedef safe_ptr<Algorithm> SafeAlgorithm;
