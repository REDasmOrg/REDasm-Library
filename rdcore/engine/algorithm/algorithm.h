#pragma once

#include <rdapi/disassembler.h>
#include <optional>
#include "../../support/safe_ptr.h"
#include "statemachine.h"

class DocumentNet;
class EmulateResult;

class Algorithm: public StateMachine
{
    private:
        enum { OK, SKIP, FAIL };

    public:
        Algorithm(Disassembler* disassembler);

    protected:
        void nextAddress(rd_address address) override;

    private:
        std::optional<rd_address> decodeAddress(rd_address address);
        bool canBeDisassembled(rd_address address, RDBlock* block) const;
        rd_address processDelaySlots(rd_address address, size_t ds);
        void processResult(EmulateResult* result);
        void processBranches(DocumentNet* net, rd_type forktype, rd_address fromaddress, rd_address address, const RDSegment* segment);

    private:
        mutable RDSegment m_currentsegment{ };
};

typedef safe_ptr<Algorithm> SafeAlgorithm;
