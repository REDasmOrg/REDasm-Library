#pragma once

#include <optional>
#include <rdapi/buffer.h>
#include "../../document/document_fwd.h"
#include "../../support/safe_ptr.h"
#include "../../object.h"
#include "emulateresult.h"

class DocumentNet;
class EmulateResult;

class Algorithm: public Object
{
    public:
        Algorithm(Context* ctx);
        bool hasNext() const;
        void enqueue(rd_address address);
        void schedule(rd_address address);
        void disassembleBlock(const RDBlock* block);
        void disassemble();
        void next();

    protected:
        void nextAddress(rd_address address);

    private:
        std::optional<rd_address> decode(rd_address address);
        std::optional<rd_address> decode(RDBufferView* view, EmulateResult* result);
        bool getNext(rd_address* address);
        bool isAddressValid(rd_address address) const;
        void processResult(EmulateResult* result);
        void processBranches(rd_type forktype, rd_address fromaddress, const EmulateResult::Value& v, const RDSegment* segment);
        void processCalls(rd_type forktype, rd_address fromaddress, const EmulateResult::Value& v, const RDSegment* segment);
        void processBranchTable(rd_address fromaddress, const EmulateResult::Value& v);
        void processCallTable(rd_address fromaddress, const EmulateResult::Value& v);
        void processTable(rd_address fromaddress, const EmulateResult::Value& v);
        rd_address processDelaySlots(rd_address address, size_t ds);

    private:
        std::deque<rd_address> m_pending;
        SafeDocument& m_document;
        DocumentNet* m_net;
};

typedef safe_ptr<Algorithm> SafeAlgorithm;
