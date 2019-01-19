#include "dalvik.h"
#include "../../formats/dex/dex.h"
#include "dalvik_printer.h"
#include "dalvik_opcodes.h"
#include "dalvik_metadata.h"
#include "dalvik_algorithm.h"

#define SET_DECODE_OPCODE_TO(opcode) m_opcodedispatcher[0x##opcode] = &DalvikAssembler::decode##opcode;

#define SET_DECODE_TO(op) SET_DECODE_OPCODE_TO(op##0); SET_DECODE_OPCODE_TO(op##1); SET_DECODE_OPCODE_TO(op##2); SET_DECODE_OPCODE_TO(op##3); \
                          SET_DECODE_OPCODE_TO(op##4); SET_DECODE_OPCODE_TO(op##5); SET_DECODE_OPCODE_TO(op##6); SET_DECODE_OPCODE_TO(op##7); \
                          SET_DECODE_OPCODE_TO(op##8); SET_DECODE_OPCODE_TO(op##9); SET_DECODE_OPCODE_TO(op##A); SET_DECODE_OPCODE_TO(op##B); \
                          SET_DECODE_OPCODE_TO(op##C); SET_DECODE_OPCODE_TO(op##D); SET_DECODE_OPCODE_TO(op##E); SET_DECODE_OPCODE_TO(op##F)

namespace REDasm {

ValuedDispatcher<instruction_id_t, bool, BufferRef&, const InstructionPtr&> DalvikAssembler::m_opcodedispatcher;

DalvikAssembler::DalvikAssembler(): AssemblerPlugin()
{
    if(!m_opcodedispatcher.empty())
        return;

    SET_DECODE_TO(0); SET_DECODE_TO(1); SET_DECODE_TO(2); SET_DECODE_TO(3);
    SET_DECODE_TO(4); SET_DECODE_TO(5); SET_DECODE_TO(6); SET_DECODE_TO(7);
    SET_DECODE_TO(8); SET_DECODE_TO(9); SET_DECODE_TO(A); SET_DECODE_TO(B);
    SET_DECODE_TO(C); SET_DECODE_TO(D); SET_DECODE_TO(E); SET_DECODE_TO(F);
}

std::string DalvikAssembler::name() const { return "Dalvik VM"; }
Printer *DalvikAssembler::createPrinter(DisassemblerAPI *disassembler) const  { return new DalvikPrinter(disassembler); }
AssemblerAlgorithm *DalvikAssembler::createAlgorithm(DisassemblerAPI *disassembler) { return new DalvikAlgorithm(disassembler, this); }

bool DalvikAssembler::decodeInstruction(const BufferRef& buffer, const InstructionPtr &instruction)
{
    instruction->id = *buffer;

    auto it = m_opcodedispatcher.find(instruction->id);

    if(it == m_opcodedispatcher.end())
    {
        REDasm::log("Cannot find opcode " + REDasm::hex(instruction->id));
        return false;
    }

    BufferRef br = buffer.advance(1);
    bool res = it->second(br, instruction);

    if(!res)
        instruction->size = sizeof(u16); // Dalvik uses always 16-bit aligned instructions

    return res;
}

bool DalvikAssembler::decodeOp0(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id, u32 type)
{
    RE_UNUSED(buffer);
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->type = type;
    instruction->size = sizeof(u16);
    return true;
}

bool DalvikAssembler::decodeOp1(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id, u32 type)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->type = type;
    instruction->size = sizeof(u16);
    instruction->reg(*buffer & 0xF);
    return true;
}

bool DalvikAssembler::decodeOp2(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16);
    instruction->reg(*buffer & 0xF);
    instruction->reg((*buffer & 0xF0) >> 4);
    return true;
}

bool DalvikAssembler::decodeOp3(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id, u32 type)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->type = type;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*buffer++);
    instruction->reg(*buffer++);
    instruction->reg(*buffer);
    return true;
}

bool DalvikAssembler::decodeOp2_s(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*buffer++);
    instruction->imm(static_cast<u16>(buffer), DalvikOperands::StringIndex);
    return true;
}

bool DalvikAssembler::decodeOp2_t(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*buffer++);
    instruction->imm(static_cast<u16>(buffer), DalvikOperands::TypeIndex);
    return true;
}

bool DalvikAssembler::decodeOp2_f(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id, u32 type)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->type = type;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*buffer++);
    instruction->imm(static_cast<u16>(buffer), DalvikOperands::FieldIndex);
    return true;
}

bool DalvikAssembler::decodeOp2_16(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*buffer++);
    instruction->reg(static_cast<u16>(buffer));
    return true;
}

bool DalvikAssembler::decodeOp2_imm4(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16);
    instruction->reg(*buffer & 0xF);
    instruction->imm((*buffer & 0xF0) >> 4);
    return true;
}

bool DalvikAssembler::decodeOp2_imm16(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*buffer++);
    instruction->imm(static_cast<u16>(buffer));
    return true;
}

bool DalvikAssembler::decodeOp2_imm32(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 3;
    instruction->reg(*buffer++);
    instruction->imm(static_cast<u32>(buffer));
    return true;
}

bool DalvikAssembler::decodeOp3_f(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id, u32 type)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->type = type;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*buffer & 0xF);
    instruction->reg((*buffer++ & 0xF0) >> 4);
    instruction->imm(static_cast<u16>(buffer), DalvikOperands::FieldIndex);
    return true;
}

bool DalvikAssembler::decodeOp3_t(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id, u32 type)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->type = type;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*buffer & 0xF);
    instruction->reg((*buffer++ & 0xF0) >> 4);
    instruction->imm(static_cast<u16>(buffer), DalvikOperands::TypeIndex);
    return true;
}

bool DalvikAssembler::decodeOp3_imm8(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*buffer++);
    instruction->reg(*buffer++);
    instruction->imm(*buffer);
    return true;
}

bool DalvikAssembler::decodeOp3_imm16(BufferRef &buffer, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*buffer & 0xF);
    instruction->reg((*buffer++ & 0xF0) >> 4);
    instruction->imm(static_cast<u16>(buffer));
    return true;
}

bool DalvikAssembler::decodeIfOp2(BufferRef &buffer, const InstructionPtr &instruction, const std::string &cond, instruction_id_t id)
{
    instruction->mnemonic = "if-" + cond;
    instruction->id = id;
    instruction->type = InstructionTypes::Jump | InstructionTypes::Conditional;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*buffer++ & 0xF);
    instruction->imm(instruction->address + (sizeof(u16) * static_cast<s16>(buffer)));
    instruction->targetOp(1);
    return true;
}

bool DalvikAssembler::decodeIfOp3(BufferRef &buffer, const InstructionPtr &instruction, const std::string &cond, instruction_id_t id)
{
    instruction->mnemonic = "if-" + cond;
    instruction->id = id;
    instruction->type = InstructionTypes::Jump | InstructionTypes::Conditional;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*buffer & 0xF);
    instruction->reg((*buffer++ & 0xF0) >> 4);
    instruction->imm(instruction->address + (sizeof(u16) * static_cast<s16>(buffer)));
    instruction->targetOp(2);
    return true;
}

bool DalvikAssembler::decodeInvoke(BufferRef &buffer, const InstructionPtr &instruction, const std::string &kind, instruction_id_t id)
{
    u8 firstb = *buffer++;
    u8 argc = firstb >> 4;
    bool needslast = false;

    instruction->size = sizeof(u16) * 2;

    if((argc > 4) && ((argc % 4) == 1))
    {
        needslast = true;
        argc--;
    }

    u16 midx = buffer;

    if(argc)
    {
        buffer.advance(sizeof(u16));
        u16 argwords = std::max(1, argc / 4);
        instruction->size += sizeof(u16) * argwords;

        for(u16 argword = 0, c = 0; (c < argc) && (argword < argwords); argword++)
        {
            u16 word = buffer;

            for(u8 i = 0; (c < argc) && (i < (4 * 8)); i += 4, c++)
            {
                register_t reg = (word & (0xF << i)) >> i;
                u64 regtype = DalvikOperands::Normal;

                if(!c)
                    regtype |= DalvikOperands::ParameterFirst;

                if(!needslast && (c == (argc - 1)))
                    regtype |= DalvikOperands::ParameterLast;

                instruction->reg(reg, regtype);
            }
        }
    }

    if(needslast)
        instruction->reg(firstb & 0xF, DalvikOperands::ParameterLast);

    instruction->imm(midx, DalvikOperands::MethodIndex);
    instruction->type = InstructionTypes::Call;
    instruction->mnemonic = "invoke-" + kind;
    return true;
}

bool DalvikAssembler::decode00(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp0(buffer, instruction, "nop", DalvikOpcodes::Nop); }
bool DalvikAssembler::decode01(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "move", DalvikOpcodes::Move); }
bool DalvikAssembler::decode02(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_16(buffer, instruction, "move/from16", DalvikOpcodes::MoveFrom16); }

bool DalvikAssembler::decode03(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode04(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode05(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_16(buffer, instruction, "move-wide/from16", DalvikOpcodes::MoveWideFrom16); }

bool DalvikAssembler::decode06(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode07(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "move-object", DalvikOpcodes::MoveObject); }
bool DalvikAssembler::decode08(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_16(buffer, instruction, "move-object/from16", DalvikOpcodes::MoveObjectFrom16); }

bool DalvikAssembler::decode09(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode0A(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp1(buffer, instruction, "move-result", DalvikOpcodes::MoveResult); }
bool DalvikAssembler::decode0B(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp1(buffer, instruction, "move-result-wide", DalvikOpcodes::MoveResultWide); }
bool DalvikAssembler::decode0C(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp1(buffer, instruction, "move-result-object", DalvikOpcodes::MoveResultObject) ;}
bool DalvikAssembler::decode0D(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp1(buffer, instruction, "move-exception", DalvikOpcodes::MoveException); }
bool DalvikAssembler::decode0E(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp0(buffer, instruction, "return-void", DalvikOpcodes::ReturnVoid, InstructionTypes::Stop); }
bool DalvikAssembler::decode0F(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp1(buffer, instruction, "return", DalvikOpcodes::Return, InstructionTypes::Stop); }
bool DalvikAssembler::decode10(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp1(buffer, instruction, "return-wide", DalvikOpcodes::ReturnWide, InstructionTypes::Stop); }
bool DalvikAssembler::decode11(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp1(buffer, instruction, "return-object", DalvikOpcodes::ReturnObject, InstructionTypes::Stop); }

bool DalvikAssembler::decode12(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_imm4(buffer, instruction, "const/4", DalvikOpcodes::Const_4); }
bool DalvikAssembler::decode13(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_imm16(buffer, instruction, "const/16", DalvikOpcodes::Const_16); }
bool DalvikAssembler::decode14(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_imm32(buffer, instruction, "const", DalvikOpcodes::Const); }
bool DalvikAssembler::decode15(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_imm16(buffer, instruction, "const-high/16", DalvikOpcodes::ConstHigh16); }
bool DalvikAssembler::decode16(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_imm16(buffer, instruction, "const-wide/16", DalvikOpcodes::ConstWide_16); }
bool DalvikAssembler::decode17(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_imm32(buffer, instruction, "const-wide/32", DalvikOpcodes::ConstWide_32);  }

bool DalvikAssembler::decode18(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode19(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_imm16(buffer, instruction, "const-wide-high/16", DalvikOpcodes::ConstWideHigh16); }
bool DalvikAssembler::decode1A(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_s(buffer, instruction, "const-string", DalvikOpcodes::ConstString); }

bool DalvikAssembler::decode1B(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode1C(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_t(buffer, instruction, "const-class", DalvikOpcodes::ConstClass); }
bool DalvikAssembler::decode1D(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp1(buffer, instruction, "monitor-enter", DalvikOpcodes::MonitorEnter); }
bool DalvikAssembler::decode1E(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp0(buffer, instruction, "monitor-exit", DalvikOpcodes::MonitorExit); }
bool DalvikAssembler::decode1F(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_t(buffer, instruction, "check-cast", DalvikOpcodes::CheckCast); }
bool DalvikAssembler::decode20(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_t(buffer, instruction, "instance-of", DalvikOpcodes::InstanceOf); }
bool DalvikAssembler::decode21(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "array-length", DalvikOpcodes::ArrayLength); }
bool DalvikAssembler::decode22(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_t(buffer, instruction, "new-instance", DalvikOpcodes::NewInstance); }
bool DalvikAssembler::decode23(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_t(buffer, instruction, "new-array", DalvikOpcodes::NewArray); }

bool DalvikAssembler::decode24(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode25(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode26(BufferRef &buffer, const InstructionPtr &instruction)
{
    decodeOp2_imm32(buffer, instruction, "fill-array-data", DalvikOpcodes::FillArrayData);

    Operand& op = instruction->op(1);
    op.u_value = instruction->address + (op.u_value * sizeof(u16)); // Offset in 16-bit code unitts
    return true;
}

bool DalvikAssembler::decode27(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp1(buffer, instruction, "throw-vx", DalvikOpcodes::Throw); }

bool DalvikAssembler::decode28(BufferRef &buffer, const InstructionPtr &instruction)
{
    instruction->mnemonic = "goto";
    instruction->id = DalvikOpcodes::Goto;
    instruction->type = InstructionTypes::Jump;
    instruction->size = sizeof(u16);
    instruction->imm(instruction->address + (static_cast<s8>(*buffer) * sizeof(u16)));
    instruction->targetOp(0);
    return true;
}

bool DalvikAssembler::decode29(BufferRef &buffer, const InstructionPtr &instruction)
{
    instruction->mnemonic = "goto/16";
    instruction->id = DalvikOpcodes::Goto_16;
    instruction->type = InstructionTypes::Jump;
    instruction->size = sizeof(u16) * 2;
    instruction->imm(instruction->address + (static_cast<s16>(buffer) * sizeof(u16)));
    instruction->targetOp(0);
    return true;
}

bool DalvikAssembler::decode2A(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode2B(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode2C(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode2D(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "cmpl-float", DalvikOpcodes::CmplFloat); }
bool DalvikAssembler::decode2E(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "cmpg-float", DalvikOpcodes::CmpgFloat); }
bool DalvikAssembler::decode2F(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "cmpl-double", DalvikOpcodes::CmplDouble); }
bool DalvikAssembler::decode30(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "cmpg-double", DalvikOpcodes::CmpgDouble); }
bool DalvikAssembler::decode31(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "cmp-long", DalvikOpcodes::CmpLong); }
bool DalvikAssembler::decode32(BufferRef &buffer, const InstructionPtr &instruction) { return decodeIfOp3(buffer, instruction, "eq", DalvikOpcodes::IfEq); }
bool DalvikAssembler::decode33(BufferRef &buffer, const InstructionPtr &instruction) { return decodeIfOp3(buffer, instruction, "ne", DalvikOpcodes::IfNe); }
bool DalvikAssembler::decode34(BufferRef &buffer, const InstructionPtr &instruction) { return decodeIfOp3(buffer, instruction, "lt", DalvikOpcodes::IfLt); }
bool DalvikAssembler::decode35(BufferRef &buffer, const InstructionPtr &instruction) { return decodeIfOp3(buffer, instruction, "ge", DalvikOpcodes::IfGe); }
bool DalvikAssembler::decode36(BufferRef &buffer, const InstructionPtr &instruction) { return decodeIfOp3(buffer, instruction, "gt", DalvikOpcodes::IfGt); }
bool DalvikAssembler::decode37(BufferRef &buffer, const InstructionPtr &instruction) { return decodeIfOp3(buffer, instruction, "le", DalvikOpcodes::IfLe); }
bool DalvikAssembler::decode38(BufferRef &buffer, const InstructionPtr &instruction) { return decodeIfOp3(buffer, instruction, "eqz", DalvikOpcodes::IfEqz); }
bool DalvikAssembler::decode39(BufferRef &buffer, const InstructionPtr &instruction) { return decodeIfOp2(buffer, instruction, "nez", DalvikOpcodes::IfNez); }
bool DalvikAssembler::decode3A(BufferRef &buffer, const InstructionPtr &instruction) { return decodeIfOp2(buffer, instruction, "ltz", DalvikOpcodes::IfLtz); }
bool DalvikAssembler::decode3B(BufferRef &buffer, const InstructionPtr &instruction) { return decodeIfOp2(buffer, instruction, "gez", DalvikOpcodes::IfGez); }
bool DalvikAssembler::decode3C(BufferRef &buffer, const InstructionPtr &instruction) { return decodeIfOp2(buffer, instruction, "gtz", DalvikOpcodes::IfGtz); }
bool DalvikAssembler::decode3D(BufferRef &buffer, const InstructionPtr &instruction) { return decodeIfOp2(buffer, instruction, "lez", DalvikOpcodes::IfLez); }

bool DalvikAssembler::decode3E(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode3F(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode40(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode41(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode42(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode43(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode44(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aget", DalvikOpcodes::Aget, InstructionTypes::Load); }
bool DalvikAssembler::decode45(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aget-wide", DalvikOpcodes::AgetWide, InstructionTypes::Load); }
bool DalvikAssembler::decode46(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aget-object", DalvikOpcodes::AgetObject, InstructionTypes::Load); }
bool DalvikAssembler::decode47(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aget-boolean", DalvikOpcodes::AgetBoolean, InstructionTypes::Load); }
bool DalvikAssembler::decode48(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aget-byte", DalvikOpcodes::AgetByte, InstructionTypes::Load); }
bool DalvikAssembler::decode49(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aget-char", DalvikOpcodes::AgetChar, InstructionTypes::Load); }
bool DalvikAssembler::decode4A(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aget-short", DalvikOpcodes::AgetShort, InstructionTypes::Load); }
bool DalvikAssembler::decode4B(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aput", DalvikOpcodes::Aput, InstructionTypes::Store); }
bool DalvikAssembler::decode4C(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aput-wide", DalvikOpcodes::AputWide, InstructionTypes::Store); }
bool DalvikAssembler::decode4D(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aput-object", DalvikOpcodes::AputObject, InstructionTypes::Store); }
bool DalvikAssembler::decode4E(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aput-boolean", DalvikOpcodes::AputBoolean, InstructionTypes::Store); }
bool DalvikAssembler::decode4F(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aput-byte", DalvikOpcodes::AputByte, InstructionTypes::Store); }
bool DalvikAssembler::decode50(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aput-char", DalvikOpcodes::AputChar, InstructionTypes::Store); }
bool DalvikAssembler::decode51(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "aput-short", DalvikOpcodes::AputShort, InstructionTypes::Store); }
bool DalvikAssembler::decode52(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iget", DalvikOpcodes::Iget, InstructionTypes::Load); }
bool DalvikAssembler::decode53(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iget-wide", DalvikOpcodes::IgetWide, InstructionTypes::Load); }
bool DalvikAssembler::decode54(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iget-object", DalvikOpcodes::IgetObject, InstructionTypes::Load); }
bool DalvikAssembler::decode55(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iget-boolean", DalvikOpcodes::IgetBoolean, InstructionTypes::Load); }
bool DalvikAssembler::decode56(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iget-byte", DalvikOpcodes::IgetByte, InstructionTypes::Load); }
bool DalvikAssembler::decode57(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iget-char", DalvikOpcodes::IgetChar, InstructionTypes::Load); }
bool DalvikAssembler::decode58(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iget-short", DalvikOpcodes::IgetShort, InstructionTypes::Load); }
bool DalvikAssembler::decode59(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iput", DalvikOpcodes::Iput, InstructionTypes::Store); }
bool DalvikAssembler::decode5A(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iput-wide", DalvikOpcodes::IputWide, InstructionTypes::Store); }
bool DalvikAssembler::decode5B(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iput-object", DalvikOpcodes::IputObject, InstructionTypes::Store); }
bool DalvikAssembler::decode5C(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iput-boolean", DalvikOpcodes::IputBoolean, InstructionTypes::Store); }
bool DalvikAssembler::decode5D(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iput-byte", DalvikOpcodes::IputByte, InstructionTypes::Store); }
bool DalvikAssembler::decode5E(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iput-char", DalvikOpcodes::IputChar, InstructionTypes::Store); }
bool DalvikAssembler::decode5F(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_f(buffer, instruction, "iput-short", DalvikOpcodes::IputShort, InstructionTypes::Store); }
bool DalvikAssembler::decode60(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sget", DalvikOpcodes::Sget, InstructionTypes::Load); }
bool DalvikAssembler::decode61(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sget-wide", DalvikOpcodes::SgetWide, InstructionTypes::Load); }
bool DalvikAssembler::decode62(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sget-object", DalvikOpcodes::SgetObject, InstructionTypes::Load); }
bool DalvikAssembler::decode63(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sget-boolean", DalvikOpcodes::SgetBoolean, InstructionTypes::Load); }
bool DalvikAssembler::decode64(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sget-byte", DalvikOpcodes::SgetByte, InstructionTypes::Load); }
bool DalvikAssembler::decode65(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sget-char", DalvikOpcodes::SgetChar, InstructionTypes::Load); }
bool DalvikAssembler::decode66(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sget-short", DalvikOpcodes::SgetShort, InstructionTypes::Load); }
bool DalvikAssembler::decode67(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sput", DalvikOpcodes::Sput, InstructionTypes::Store); }
bool DalvikAssembler::decode68(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sput-wide", DalvikOpcodes::SputWide, InstructionTypes::Store); }
bool DalvikAssembler::decode69(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sput-object", DalvikOpcodes::SputObject, InstructionTypes::Store); }
bool DalvikAssembler::decode6A(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sput-boolean", DalvikOpcodes::SputBoolean, InstructionTypes::Store); }
bool DalvikAssembler::decode6B(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sput-byte", DalvikOpcodes::SputByte, InstructionTypes::Store); }
bool DalvikAssembler::decode6C(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sput-char", DalvikOpcodes::SputChar, InstructionTypes::Store); }
bool DalvikAssembler::decode6D(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2_f(buffer, instruction, "sput-short", DalvikOpcodes::SputShort, InstructionTypes::Store); }
bool DalvikAssembler::decode6E(BufferRef &buffer, const InstructionPtr &instruction) { return decodeInvoke(buffer, instruction, "virtual", DalvikOpcodes::InvokeVirtual); }
bool DalvikAssembler::decode6F(BufferRef &buffer, const InstructionPtr &instruction) { return decodeInvoke(buffer, instruction, "super", DalvikOpcodes::InvokeSuper); }
bool DalvikAssembler::decode70(BufferRef &buffer, const InstructionPtr &instruction) { return decodeInvoke(buffer, instruction, "direct", DalvikOpcodes::InvokeDirect); }
bool DalvikAssembler::decode71(BufferRef &buffer, const InstructionPtr &instruction) { return decodeInvoke(buffer, instruction, "static", DalvikOpcodes::InvokeStatic); }
bool DalvikAssembler::decode72(BufferRef &buffer, const InstructionPtr &instruction) { return decodeInvoke(buffer, instruction, "interface", DalvikOpcodes::InvokeInterface); }

bool DalvikAssembler::decode73(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode74(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode75(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode76(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode77(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode78(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode79(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode7A(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode7B(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "neg-int", DalvikOpcodes::NegInt); }

bool DalvikAssembler::decode7C(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode7D(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "neg-long", DalvikOpcodes::NegLong); }

bool DalvikAssembler::decode7E(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode7F(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "neg-float", DalvikOpcodes::NegFloat); }
bool DalvikAssembler::decode80(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "neg-double", DalvikOpcodes::NegDouble); }
bool DalvikAssembler::decode81(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "int-to-long", DalvikOpcodes::IntToLong); }
bool DalvikAssembler::decode82(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "int-to-float", DalvikOpcodes::IntToFloat); }
bool DalvikAssembler::decode83(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "int-to-double", DalvikOpcodes::IntToDouble); }
bool DalvikAssembler::decode84(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "long-to-int", DalvikOpcodes::LongToInt); }
bool DalvikAssembler::decode85(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "long-to-float", DalvikOpcodes::LongToFloat); }
bool DalvikAssembler::decode86(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "long-to-double", DalvikOpcodes::LongToDouble); }
bool DalvikAssembler::decode87(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "float-to-int", DalvikOpcodes::FloatToInt); }
bool DalvikAssembler::decode88(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "float-to-long", DalvikOpcodes::FloatToLong); }
bool DalvikAssembler::decode89(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "float-to-double", DalvikOpcodes::FloatToDouble); }
bool DalvikAssembler::decode8A(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "double-to-int", DalvikOpcodes::DoubleToInt); }
bool DalvikAssembler::decode8B(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "double-to-long", DalvikOpcodes::DoubleToLong); }
bool DalvikAssembler::decode8C(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "double-to-float", DalvikOpcodes::DoubleToFloat); }
bool DalvikAssembler::decode8D(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "int-to-byte", DalvikOpcodes::IntToByte); }
bool DalvikAssembler::decode8E(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "int-to-char", DalvikOpcodes::IntToChar); }
bool DalvikAssembler::decode8F(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "int-to-short", DalvikOpcodes::IntToShort); }
bool DalvikAssembler::decode90(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "add-int", DalvikOpcodes::AddInt); }
bool DalvikAssembler::decode91(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "sub-int", DalvikOpcodes::SubInt); }
bool DalvikAssembler::decode92(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "mul-int", DalvikOpcodes::MulInt); }
bool DalvikAssembler::decode93(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "div-int", DalvikOpcodes::DivInt); }
bool DalvikAssembler::decode94(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "rem-int", DalvikOpcodes::RemInt); }
bool DalvikAssembler::decode95(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "and-int", DalvikOpcodes::AndInt); }
bool DalvikAssembler::decode96(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "or-int", DalvikOpcodes::OrInt); }
bool DalvikAssembler::decode97(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "xor-int", DalvikOpcodes::XorInt); }
bool DalvikAssembler::decode98(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "shl-int", DalvikOpcodes::ShlInt); }
bool DalvikAssembler::decode99(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "shr-int", DalvikOpcodes::ShrInt); }
bool DalvikAssembler::decode9A(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "ushr-int", DalvikOpcodes::UshrInt); }
bool DalvikAssembler::decode9B(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "add-long", DalvikOpcodes::AddLong); }
bool DalvikAssembler::decode9C(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "sub-long", DalvikOpcodes::SubLong); }
bool DalvikAssembler::decode9D(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "mul-long", DalvikOpcodes::MulLong); }
bool DalvikAssembler::decode9E(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "div-long", DalvikOpcodes::DivLong); }
bool DalvikAssembler::decode9F(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "rem-long", DalvikOpcodes::RemLong); }
bool DalvikAssembler::decodeA0(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "and-long", DalvikOpcodes::AndLong); }
bool DalvikAssembler::decodeA1(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "or-long", DalvikOpcodes::OrLong); }
bool DalvikAssembler::decodeA2(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "xor-long", DalvikOpcodes::XorLong); }
bool DalvikAssembler::decodeA3(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "shl-long", DalvikOpcodes::ShlLong); }
bool DalvikAssembler::decodeA4(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "shr-long", DalvikOpcodes::ShrLong); }
bool DalvikAssembler::decodeA5(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "ushr-long", DalvikOpcodes::UshrLong); }
bool DalvikAssembler::decodeA6(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "add-float", DalvikOpcodes::AddFloat); }
bool DalvikAssembler::decodeA7(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "sub-float", DalvikOpcodes::SubFloat); }
bool DalvikAssembler::decodeA8(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "mul-float", DalvikOpcodes::MulFloat); }
bool DalvikAssembler::decodeA9(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "div-float", DalvikOpcodes::DivFloat); }
bool DalvikAssembler::decodeAA(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "rem-float", DalvikOpcodes::RemFloat); }
bool DalvikAssembler::decodeAB(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "add-double", DalvikOpcodes::AddDouble); }
bool DalvikAssembler::decodeAC(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "sub-double", DalvikOpcodes::SubDouble); }
bool DalvikAssembler::decodeAD(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "mul-double", DalvikOpcodes::MulDouble); }
bool DalvikAssembler::decodeAE(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "div-double", DalvikOpcodes::DivDouble); }
bool DalvikAssembler::decodeAF(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3(buffer, instruction, "rem-double", DalvikOpcodes::RemDouble); }
bool DalvikAssembler::decodeB0(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "add-int/2addr", DalvikOpcodes::AddInt_2Addr); }
bool DalvikAssembler::decodeB1(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "sub-int/2addr", DalvikOpcodes::SubInt_2Addr); }
bool DalvikAssembler::decodeB2(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "mul-int/2addr", DalvikOpcodes::MulInt_2Addr); }
bool DalvikAssembler::decodeB3(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "div-int/2addr", DalvikOpcodes::DivInt_2Addr); }
bool DalvikAssembler::decodeB4(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "rem-int/2addr", DalvikOpcodes::RemInt_2Addr); }
bool DalvikAssembler::decodeB5(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "and-int/2addr", DalvikOpcodes::AndInt_2Addr); }
bool DalvikAssembler::decodeB6(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "or-int/2addr", DalvikOpcodes::OrInt_2Addr); }
bool DalvikAssembler::decodeB7(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "xor-int/2addr", DalvikOpcodes::XorInt_2Addr); }
bool DalvikAssembler::decodeB8(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "shl-int/2addr", DalvikOpcodes::ShlInt_2Addr); }
bool DalvikAssembler::decodeB9(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "shr-int/2addr", DalvikOpcodes::ShrInt_2Addr); }
bool DalvikAssembler::decodeBA(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "ushr-int/2addr", DalvikOpcodes::UshrInt_2Addr); }
bool DalvikAssembler::decodeBB(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "add-long/2addr", DalvikOpcodes::AddLong_2Addr); }
bool DalvikAssembler::decodeBC(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "sub-long/2addr", DalvikOpcodes::SubLong_2Addr); }
bool DalvikAssembler::decodeBD(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "mul-long/2addr", DalvikOpcodes::MulLong_2Addr); }
bool DalvikAssembler::decodeBE(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "div-long/2addr", DalvikOpcodes::DivLong_2Addr); }
bool DalvikAssembler::decodeBF(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "rem-long/2addr", DalvikOpcodes::RemLong_2Addr); }
bool DalvikAssembler::decodeC0(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "and-long/2addr", DalvikOpcodes::AndLong_2Addr); }
bool DalvikAssembler::decodeC1(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "or-long/2addr", DalvikOpcodes::OrLong_2Addr); }
bool DalvikAssembler::decodeC2(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "xor-long/2addr", DalvikOpcodes::XorLong_2Addr); }
bool DalvikAssembler::decodeC3(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "shl-long/2addr", DalvikOpcodes::ShlLong_2Addr); }
bool DalvikAssembler::decodeC4(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "shr-long/2addr", DalvikOpcodes::ShrLong_2Addr); }
bool DalvikAssembler::decodeC5(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "ushr-long/2addr", DalvikOpcodes::UshrLong_2Addr); }
bool DalvikAssembler::decodeC6(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "add-float/2addr", DalvikOpcodes::AddFloat_2Addr); }
bool DalvikAssembler::decodeC7(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "sub-float/2addr", DalvikOpcodes::SubFloat_2Addr); }
bool DalvikAssembler::decodeC8(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "mul-float/2addr", DalvikOpcodes::MulFloat_2Addr); }
bool DalvikAssembler::decodeC9(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "div-float/2addr", DalvikOpcodes::DivFloat_2Addr); }
bool DalvikAssembler::decodeCA(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "rem-float/2addr", DalvikOpcodes::RemFloat_2Addr); }
bool DalvikAssembler::decodeCB(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "add-double/2addr", DalvikOpcodes::AddDouble_2Addr); }
bool DalvikAssembler::decodeCC(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "sub-double/2addr", DalvikOpcodes::SubDouble_2Addr); }
bool DalvikAssembler::decodeCD(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "mul-double/2addr", DalvikOpcodes::MulDouble_2Addr); }
bool DalvikAssembler::decodeCE(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "div-double/2addr", DalvikOpcodes::DivDouble_2Addr); }
bool DalvikAssembler::decodeCF(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp2(buffer, instruction, "rem-double/2addr", DalvikOpcodes::RemDouble_2Addr); }
bool DalvikAssembler::decodeD0(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm16(buffer, instruction, "add-int/lit16", DalvikOpcodes::AddIntLit16); }
bool DalvikAssembler::decodeD1(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm16(buffer, instruction, "sub-int/lit16", DalvikOpcodes::SubIntLit16); }
bool DalvikAssembler::decodeD2(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm16(buffer, instruction, "mul-int/lit16", DalvikOpcodes::MulIntLit16); }
bool DalvikAssembler::decodeD3(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm16(buffer, instruction, "div-int/lit16", DalvikOpcodes::DivIntLit16); }
bool DalvikAssembler::decodeD4(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm16(buffer, instruction, "rem-int/lit16", DalvikOpcodes::RemIntLit16); }
bool DalvikAssembler::decodeD5(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm16(buffer, instruction, "and-int/lit16", DalvikOpcodes::AndIntLit16); }
bool DalvikAssembler::decodeD6(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm16(buffer, instruction, "or-int/lit16", DalvikOpcodes::OrIntLit16); }
bool DalvikAssembler::decodeD7(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm16(buffer, instruction, "xor-int/lit16", DalvikOpcodes::XorIntLit16); }
bool DalvikAssembler::decodeD8(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm8(buffer, instruction, "add-int/lit8", DalvikOpcodes::AddIntLit8); }
bool DalvikAssembler::decodeD9(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm8(buffer, instruction, "sub-int/lit8", DalvikOpcodes::SubIntLit8); }
bool DalvikAssembler::decodeDA(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm8(buffer, instruction, "mul-int/lit8", DalvikOpcodes::MulIntLit8); }
bool DalvikAssembler::decodeDB(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm8(buffer, instruction, "div-int/lit8", DalvikOpcodes::DivIntLit8); }
bool DalvikAssembler::decodeDC(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm8(buffer, instruction, "rem-int/lit8", DalvikOpcodes::RemIntLit8); }
bool DalvikAssembler::decodeDD(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm8(buffer, instruction, "and-int/lit8", DalvikOpcodes::AndIntLit8); }
bool DalvikAssembler::decodeDE(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm8(buffer, instruction, "or-int/lit8", DalvikOpcodes::OrIntLit8); }
bool DalvikAssembler::decodeDF(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm8(buffer, instruction, "xor-int/lit8", DalvikOpcodes::XorIntLit8); }
bool DalvikAssembler::decodeE0(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm8(buffer, instruction, "shl-int/lit8", DalvikOpcodes::ShlIntLit8); }
bool DalvikAssembler::decodeE1(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm8(buffer, instruction, "shr-int/lit8", DalvikOpcodes::ShrIntLit8); }
bool DalvikAssembler::decodeE2(BufferRef &buffer, const InstructionPtr &instruction) { return decodeOp3_imm8(buffer, instruction, "ushr-int/lit8", DalvikOpcodes::UshrIntLit8); }

bool DalvikAssembler::decodeE3(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeE4(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeE5(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeE6(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeE7(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeE8(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeE9(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeEA(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeEB(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeEC(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeED(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeEE(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeEF(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF0(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF1(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF2(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF3(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF4(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF5(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF6(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF7(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF8(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF9(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeFA(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeFB(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeFC(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeFD(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeFE(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeFF(BufferRef &buffer, const InstructionPtr &instruction)
{
    return false;
}

} // namespace REDasm
