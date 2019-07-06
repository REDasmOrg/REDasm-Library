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
        AlgorithmImpl(Algorithm* algorithm, Disassembler* disassembler);
        size_t disassembleInstruction(address_t address, const CachedInstruction &instruction);
        void done(address_t address);
        void enqueue(address_t address);
        void analyze();

    protected:
        bool validateState(const State& state) const override;
        void onNewState(const State *state) const override;

    private:
        void loadTargets(const CachedInstruction &instruction);
        void validateTarget(const CachedInstruction &instruction) const;
        bool canBeDisassembled(address_t address);
        void createInvalidInstruction(const CachedInstruction &instruction);
        size_t disassemble(address_t address, const CachedInstruction &instruction);
        void emulateOperand(const Operand* op, const CachedInstruction &instruction);
        void emulate(const CachedInstruction& instruction);

    private:
        //std::unique_ptr<Emulator> m_emulator;
        ListingDocument& m_document;
        Disassembler* m_disassembler;
        Assembler* m_assembler;
        Loader* m_loader;
        DecodedAddresses m_done;
        Analyzer* m_analyzer;
        const Segment* m_currentsegment;
        bool m_analyzed;
};

} // namespace REDasm
