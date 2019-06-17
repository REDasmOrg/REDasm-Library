#pragma once

#include <redasm/types/api_types.h>
#include <redasm/types/callback_types.h>
#include <redasm/pimpl.h>
#include <string>
#include <deque>

namespace REDasm {

class InstructionImpl
{
    PIMPL_DECLARE_Q(Instruction)
    PIMPL_DECLARE_PUBLIC(Instruction)

    public:
        InstructionImpl(Instruction* q);
        ~InstructionImpl();
        const char* mnemonic() const;
        address_t address() const;
        address_t endAddress() const;
        u32 size() const;
        instruction_id_t id() const;
        const InstructionType& type() const;
        InstructionType& type();

    public:
        size_t operandsCount() const;
        size_t opSize(size_t idx) const;
        const Operand* op(size_t idx) const;
        Operand* op(size_t idx);
        const Operand *target() const;
        void opSize(size_t idx, size_t size);
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
        size_t targetsCount() const;
        address_t targetAt(size_t idx) const;
        void target(address_t address);
        void targetIdx(size_t idx);
        void clearTargets();

    public:
        bool is(InstructionType t) const;
        bool isInvalid() const;
        void* userData() const;
        void reset();

    public:
        void setMnemonic(const char* s);
        void setAddress(address_t address);
        void setSize(u32 size);
        void setId(instruction_id_t id);
        void setType(InstructionType type);
        void setFree(const Callback_VoidPointer& cb);
        void setUserData(void* userdata);

    private: // Serializable types
        std::string m_mnemonic;
        std::deque<Operand> m_operands;
        address_t m_address;
        InstructionType m_type;
        u32 m_size;
        instruction_id_t m_id;

     private:
        Callback_VoidPointer m_free;
        std::set<address_t> m_utargets;
        std::deque<address_t> m_targets;
        void* m_userdata;

     friend class Serializer<Instruction>;
};

} // namespace REDasm
