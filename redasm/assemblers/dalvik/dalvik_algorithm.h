#ifndef DALVIK_ALGORITHM_H
#define DALVIK_ALGORITHM_H

#include "../../plugins/assembler/algorithm/algorithm.h"

namespace REDasm {

class DEXFormat;

class DalvikAlgorithm: public AssemblerAlgorithm
{
    DEFINE_STATES(StringIndexState = UserState, MethodIndexState)

    public:
        DalvikAlgorithm(DisassemblerAPI* disassembler, AssemblerPlugin* assemblerplugin);

    protected:
        virtual void validateTarget(const InstructionPtr&) const;
        virtual void onDecodeFailed(const InstructionPtr& instruction);
        virtual void onDecodedOperand(const Operand& op, const InstructionPtr& instruction);
        virtual void onDecoded(const InstructionPtr& instruction);
        virtual void decodeState(State *state);
        virtual void stringIndexState(const State* state);
        virtual void methodIndexState(const State* state);

    private:
        void checkImport(const State *state);

    private:
        DEXFormat* m_dexformat;
        std::unordered_set<std::string> m_imports;
        std::unordered_set<address_t> m_methodbounds;
};

} // namespace REDasm

#endif // DALVIK_ALGORITHM_H
