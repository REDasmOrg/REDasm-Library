#ifndef DALVIK_ALGORITHM_H
#define DALVIK_ALGORITHM_H

#include "../../plugins/assembler/algorithm/algorithm.h"

namespace REDasm {

class DEXLoader;
class DEXDebugInfo;
class DEXEncodedMethod;

class DalvikAlgorithm: public AssemblerAlgorithm
{
    DEFINE_STATES(StringIndexState = UserState, MethodIndexState,
                  PackedSwitchTableState, SparseSwitchTableState, FillArrayDataState,
                  DebugInfoState)

    private:
        typedef std::unordered_map<address_t, std::list<s32> > PackagedCaseMap;
        typedef std::unordered_map<u64, address_t> SparseCaseMap;

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
        virtual void debugInfoState(const State* state);

    private:
        void emitCaseInfo(address_t address, const PackagedCaseMap& casemap);
        void emitCaseInfo(address_t address, const InstructionPtr &instruction, const SparseCaseMap& casemap);
        void emitArguments(const State* state, const DEXEncodedMethod &dexmethod, const DEXDebugInfo &dexdebuginfo);
        void emitDebugData(const DEXDebugInfo &dexdebuginfo);
        void checkImport(const State *state);
        bool canContinue(const InstructionPtr& instruction);

    private:
        DEXLoader* m_dexloader;
        std::unordered_set<std::string> m_imports;
        std::unordered_set<address_t> m_methodbounds;
};

} // namespace REDasm

#endif // DALVIK_ALGORITHM_H
