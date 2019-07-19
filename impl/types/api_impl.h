#pragma once

#include <redasm/types/api.h>
#include <redasm/types/containers/list.h>
#include <redasm/types/callback.h>
#include <redasm/pimpl.h>
#include <deque>
#include <set>

namespace REDasm {

class OperandImpl
{
    PIMPL_DECLARE_Q(Operand)
    PIMPL_DECLARE_PUBLIC(Operand)

    public:
        OperandImpl();
        OperandImpl(OperandType type, s64 value, size_t idx, tag_t tag);
        OperandImpl(OperandType type, u64 value, size_t idx, tag_t tag);

    private:
        OperandType m_type;
        tag_t m_tag;
        u32 m_size;
        size_t m_index, m_locindex;
        RegisterOperand m_reg;
        DisplacementOperand m_disp;
        union { s64 m_svalue; u64 m_uvalue; };
};

class InstructionImpl
{
    PIMPL_DECLARE_Q(Instruction)
    PIMPL_DECLARE_PUBLIC(Instruction)

    public:
        InstructionImpl(Instruction* q);
        ~InstructionImpl();

    public:
        bool hasOperands() const;
        size_t operandsCount() const;
        const Operand *target() const;
        const Operand* op(size_t idx) const;
        Operand* op(size_t idx);
        Instruction* mem(address_t v, tag_t tag);
        Instruction* cnst(u64 v, tag_t tag);
        Instruction* imm(u64 v, tag_t tag);
        Instruction* disp(register_id_t base, s64 displacement);
        Instruction* disp(register_id_t base, register_id_t index, s64 displacement);
        Instruction* disp(register_id_t base, register_id_t index, s64 scale, s64 displacement);
        Instruction* arg(size_t locindex, register_id_t base, register_id_t index, s64 displacement);
        Instruction* local(size_t locindex, register_id_t base, register_id_t index, s64 displacement, OperandType type);
        Instruction* reg(register_id_t r, tag_t tag);

    public:
        const List& targets() const;
        void target(address_t address);
        void targetIdx(size_t idx);

    public:
        void* userData() const;
        void reset();

    public:
        void setFree(const Callback_VoidPointer& cb);
        void setUserData(void* userdata);

     private:
        Callback_VoidPointer m_free;
        std::set<address_t> m_utargets;
        List m_targets, m_operands;
        void* m_userdata;
};

} // namespace REDasm
