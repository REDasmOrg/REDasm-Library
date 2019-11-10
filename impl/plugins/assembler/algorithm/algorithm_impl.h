#pragma once

#include <memory>
#include <redasm/plugins/assembler/algorithm/algorithm.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/plugins/loader/analyzer.h>
#include <redasm/plugins/loader/loader.h>
#include <redasm/disassembler/listing/listingdocument.h>
#include "statemachine.h"

namespace REDasm {

class AlgorithmImpl: public StateMachine
{
    PIMPL_DECLARE_Q(Algorithm)
    PIMPL_DECLARE_PUBLIC(Algorithm)

    private:
        typedef std::unordered_set<address_t> DecodedAddresses;

    public:
        AlgorithmImpl(Algorithm* algorithm);
        void enqueue(address_t address);
        CachedInstruction decodeInstruction(address_t address);

    protected:
        bool validateState(const State& state) const override;
        void onNewState(const State *state) const override;

    private:
        size_t decode(address_t address, const CachedInstruction &instruction);
        CachedInstruction decode(address_t address);
        void loadTargets(const CachedInstruction &instruction);
        void validateTarget(const CachedInstruction &instruction) const;
        bool canBeDisassembled(address_t address);
        void createInvalidInstruction(const CachedInstruction &instruction);

    private:
        Analyzer* m_analyzer;
        const Segment* m_currentsegment;
        bool m_analyzed;
};

} // namespace REDasm
