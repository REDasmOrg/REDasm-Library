#pragma once

#include <optional>
#include "../../document/document_fwd.h"
#include "../../plugin/assembler.h"
#include "../../support/safe_ptr.h"
#include "../../object.h"
#include "emulateresult.h"

class DocumentNet;

class Algorithm: public Object
{
    public:
        Algorithm(Context* ctx);
        bool hasNext() const;
        void enqueue(rd_address address);
        void schedule(rd_address address);
        void disassembleBlock(const RDBlock* block);
        void disassemble();

    private:
        Assembler* getAssembler(const std::string& id);
        std::optional<rd_address> decode(rd_address address);
        std::optional<rd_address> decode(RDBufferView* view, EmulateResult* result);
        bool isAddressValid(rd_address address) const;
        void next();
        void nextAddress(rd_address address);
        void processResult(EmulateResult* result);
        void processBranches(rd_type forktype, rd_address fromaddress, const EmulateResult::Value& v, const RDSegment* segment);
        void processCalls(rd_type forktype, rd_address fromaddress, const EmulateResult::Value& v, const RDSegment* segment);
        void processBranchTable(rd_address fromaddress, const EmulateResult::Value& v);
        void processCallTable(rd_address fromaddress, const EmulateResult::Value& v);
        void processTable(rd_address fromaddress, const EmulateResult::Value& v);
        rd_address processDelaySlots(rd_address address, size_t ds);

    private:
        std::unordered_map<std::string, Assembler> m_assemblers;
        std::deque<rd_address> m_pending;
        SafeDocument& m_document;
        DocumentNet* m_net;
};

typedef safe_ptr<Algorithm> SafeAlgorithm;
