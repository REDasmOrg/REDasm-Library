#pragma once

#include <unordered_map>
#include <functional>
#include <optional>
#include <array>
#include "rdil.h"

class ILCPU: public RDIL
{
    private:
        typedef std::function<u64(u64)> UnaryOpFunc;
        typedef std::function<u64(u64, u64)> BinaryOpFunc;

    public:
        ILCPU() = default;
        static void init(RDInstruction* instruction);

    public:
        bool reg(rd_register_id r, u64* value) const;
        void exec(const RDInstruction* rdil);
        void reset();

    private:
        void exec(const RDOperand& dst, const RDOperand& op1, const UnaryOpFunc& opfunc);
        void exec(const RDOperand& dst, const RDOperand& op1, const RDOperand& op2, const BinaryOpFunc& opfunc);
        bool expect(const RDInstruction* rdil, const RDOperand& op, rd_type type);
        void fail(const std::string& msg);

    private:
        std::optional<u64> readop(const RDOperand& op) const;
        void writeop(const RDOperand& op, u64 val);

    private:
        std::array<rd_register_id, RDILRegister_Count> m_regs;
        std::unordered_map<rd_register_id, u64> m_gpr;
        bool m_failed{false};
};
