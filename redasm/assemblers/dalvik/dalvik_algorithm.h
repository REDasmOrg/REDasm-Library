#ifndef DALVIK_ALGORITHM_H
#define DALVIK_ALGORITHM_H

#include "../../plugins/assembler/algorithm/algorithm.h"

namespace REDasm {

class DEXLoader;

class DalvikAlgorithm: public AssemblerAlgorithm
{
    DEFINE_STATES(StringIndexState = UserState, MethodIndexState,
                  PackedSwitchTableState, SparseSwitchTableState, FillArrayDataState)

    private:
        typedef std::unordered_map<address_t, std::list<u32> > CaseMap;

    public:
        DalvikAlgorithm(DisassemblerAPI* disassembler, AssemblerPlugin* assemblerplugin);

    protected:
        virtual void validateTarget(const InstructionPtr&) const;
        virtual void onDecodeFailed(const InstructionPtr& instruction);
        virtual void onDecodedOperand(const Operand *op, const InstructionPtr& instruction);
        virtual void onDecoded(const InstructionPtr& instruction);
        virtual void decodeState(const State *state);
        virtual void stringIndexState(const State* state);
        virtual void methodIndexState(const State* state);
        virtual void packedSwitchTableState(const State* state);
        virtual void sparseSwitchTableState(const State* state);
        virtual void fillArrayDataState(const State* state);

    private:
        void emitCaseInfo(address_t address, const CaseMap& casemap);
        void checkImport(const State *state);
        bool canContinue(const InstructionPtr& instruction);

    private:
        DEXLoader* m_dexloader;
        std::unordered_set<std::string> m_imports;
        std::unordered_set<address_t> m_methodbounds;
};

} // namespace REDasm

#endif // DALVIK_ALGORITHM_H
