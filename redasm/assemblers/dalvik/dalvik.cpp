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

ValuedDispatcher<instruction_id_t, bool, BufferView&, const InstructionPtr&> DalvikAssembler::m_opcodedispatcher;

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

bool DalvikAssembler::decodeInstruction(const BufferView &view, const InstructionPtr &instruction)
{
    instruction->id = *view;

    auto it = m_opcodedispatcher.find(instruction->id);

    if(it == m_opcodedispatcher.end())
    {
        REDasm::log("Cannot find opcode " + REDasm::hex(instruction->id));
        return false;
    }

    BufferView subview = view + 1;
    bool res = it->second(subview, instruction);

    if(!res)
        instruction->size = sizeof(u16); // Dalvik uses always 16-bit aligned instructions

    return res;
}

bool DalvikAssembler::decodeOp0(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id, u32 type)
{
    RE_UNUSED(view);
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->type = type;
    instruction->size = sizeof(u16);
    return true;
}

bool DalvikAssembler::decodeOp1(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id, u32 type)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->type = type;
    instruction->size = sizeof(u16);
    instruction->reg(*view & 0xF);
    return true;
}

bool DalvikAssembler::decodeOp2(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16);
    instruction->reg(*view & 0xF);
    instruction->reg((*view & 0xF0) >> 4);
    return true;
}

bool DalvikAssembler::decodeOp3(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id, u32 type)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->type = type;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*view++);
    instruction->reg(*view++);
    instruction->reg(*view);
    return true;
}

bool DalvikAssembler::decodeOp2_s(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*view++);
    instruction->imm(static_cast<u16>(view), DalvikOperands::StringIndex);
    return true;
}

bool DalvikAssembler::decodeOp2_t(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*view++);
    instruction->imm(static_cast<u16>(view), DalvikOperands::TypeIndex);
    return true;
}

bool DalvikAssembler::decodeOp2_f(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id, u32 type)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->type = type;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*view++);
    instruction->imm(static_cast<u16>(view), DalvikOperands::FieldIndex);
    return true;
}

bool DalvikAssembler::decodeOp2_16(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*view++);
    instruction->reg(static_cast<u16>(view));
    return true;
}

bool DalvikAssembler::decodeOp2_imm4(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16);
    instruction->reg(*view & 0xF);
    instruction->imm((*view & 0xF0) >> 4);
    return true;
}

bool DalvikAssembler::decodeOp2_imm16(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*view++);
    instruction->imm(static_cast<u16>(view));
    return true;
}

bool DalvikAssembler::decodeOp2_imm32(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 3;
    instruction->reg(*view++);
    instruction->imm(static_cast<u32>(view));
    return true;
}

bool DalvikAssembler::decodeOp3_f(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id, u32 type)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->type = type;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*view & 0xF);
    instruction->reg((*view++ & 0xF0) >> 4);
    instruction->imm(static_cast<u16>(view), DalvikOperands::FieldIndex);
    return true;
}

bool DalvikAssembler::decodeOp3_t(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id, u32 type)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->type = type;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*view & 0xF);
    instruction->reg((*view++ & 0xF0) >> 4);
    instruction->imm(static_cast<u16>(view), DalvikOperands::TypeIndex);
    return true;
}

bool DalvikAssembler::decodeOp3_imm8(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*view++);
    instruction->reg(*view++);
    instruction->imm(*view);
    return true;
}

bool DalvikAssembler::decodeOp3_imm16(BufferView& view, const InstructionPtr &instruction, const std::string &mnemonic, instruction_id_t id)
{
    instruction->mnemonic = mnemonic;
    instruction->id = id;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*view & 0xF);
    instruction->reg((*view++ & 0xF0) >> 4);
    instruction->imm(static_cast<u16>(view));
    return true;
}

bool DalvikAssembler::decodeIfOp2(BufferView& view, const InstructionPtr &instruction, const std::string &cond, instruction_id_t id)
{
    instruction->mnemonic = "if-" + cond;
    instruction->id = id;
    instruction->type = InstructionTypes::Jump | InstructionTypes::Conditional;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*view++ & 0xF);
    instruction->imm(instruction->address + (sizeof(u16) * static_cast<s16>(view)));
    instruction->targetOp(1);
    return true;
}

bool DalvikAssembler::decodeIfOp3(BufferView& view, const InstructionPtr &instruction, const std::string &cond, instruction_id_t id)
{
    instruction->mnemonic = "if-" + cond;
    instruction->id = id;
    instruction->type = InstructionTypes::Jump | InstructionTypes::Conditional;
    instruction->size = sizeof(u16) * 2;
    instruction->reg(*view & 0xF);
    instruction->reg((*view++ & 0xF0) >> 4);
    instruction->imm(instruction->address + (sizeof(u16) * static_cast<s16>(view)));
    instruction->targetOp(2);
    return true;
}

bool DalvikAssembler::decodeInvoke(BufferView& view, const InstructionPtr &instruction, const std::string &kind, instruction_id_t id)
{
    u8 firstb = *view++;
    u8 argc = firstb >> 4;
    bool needslast = false;

    instruction->size = sizeof(u16) * 2;

    if((argc > 4) && ((argc % 4) == 1))
    {
        needslast = true;
        argc--;
    }

    u16 midx = view;

    if(argc)
    {
        view += sizeof(u16);
        u16 argwords = std::max(1, argc / 4);
        instruction->size += sizeof(u16) * argwords;

        for(u16 argword = 0, c = 0; (c < argc) && (argword < argwords); argword++)
        {
            u16 word = view;

            for(u8 i = 0; (c < argc) && (i < (4 * 8)); i += 4, c++)
            {
                register_id_t reg = (word & (0xF << i)) >> i;
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

bool DalvikAssembler::decode00(BufferView& view, const InstructionPtr &instruction) { return decodeOp0(view, instruction, "nop", DalvikOpcodes::Nop); }
bool DalvikAssembler::decode01(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "move", DalvikOpcodes::Move); }
bool DalvikAssembler::decode02(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_16(view, instruction, "move/from16", DalvikOpcodes::MoveFrom16); }

bool DalvikAssembler::decode03(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode04(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode05(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_16(view, instruction, "move-wide/from16", DalvikOpcodes::MoveWideFrom16); }

bool DalvikAssembler::decode06(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode07(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "move-object", DalvikOpcodes::MoveObject); }
bool DalvikAssembler::decode08(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_16(view, instruction, "move-object/from16", DalvikOpcodes::MoveObjectFrom16); }

bool DalvikAssembler::decode09(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode0A(BufferView& view, const InstructionPtr &instruction) { return decodeOp1(view, instruction, "move-result", DalvikOpcodes::MoveResult); }
bool DalvikAssembler::decode0B(BufferView& view, const InstructionPtr &instruction) { return decodeOp1(view, instruction, "move-result-wide", DalvikOpcodes::MoveResultWide); }
bool DalvikAssembler::decode0C(BufferView& view, const InstructionPtr &instruction) { return decodeOp1(view, instruction, "move-result-object", DalvikOpcodes::MoveResultObject) ;}
bool DalvikAssembler::decode0D(BufferView& view, const InstructionPtr &instruction) { return decodeOp1(view, instruction, "move-exception", DalvikOpcodes::MoveException); }
bool DalvikAssembler::decode0E(BufferView& view, const InstructionPtr &instruction) { return decodeOp0(view, instruction, "return-void", DalvikOpcodes::ReturnVoid, InstructionTypes::Stop); }
bool DalvikAssembler::decode0F(BufferView& view, const InstructionPtr &instruction) { return decodeOp1(view, instruction, "return", DalvikOpcodes::Return, InstructionTypes::Stop); }
bool DalvikAssembler::decode10(BufferView& view, const InstructionPtr &instruction) { return decodeOp1(view, instruction, "return-wide", DalvikOpcodes::ReturnWide, InstructionTypes::Stop); }
bool DalvikAssembler::decode11(BufferView& view, const InstructionPtr &instruction) { return decodeOp1(view, instruction, "return-object", DalvikOpcodes::ReturnObject, InstructionTypes::Stop); }

bool DalvikAssembler::decode12(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_imm4(view, instruction, "const/4", DalvikOpcodes::Const_4); }
bool DalvikAssembler::decode13(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_imm16(view, instruction, "const/16", DalvikOpcodes::Const_16); }
bool DalvikAssembler::decode14(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_imm32(view, instruction, "const", DalvikOpcodes::Const); }
bool DalvikAssembler::decode15(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_imm16(view, instruction, "const-high/16", DalvikOpcodes::ConstHigh16); }
bool DalvikAssembler::decode16(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_imm16(view, instruction, "const-wide/16", DalvikOpcodes::ConstWide_16); }
bool DalvikAssembler::decode17(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_imm32(view, instruction, "const-wide/32", DalvikOpcodes::ConstWide_32);  }

bool DalvikAssembler::decode18(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode19(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_imm16(view, instruction, "const-wide-high/16", DalvikOpcodes::ConstWideHigh16); }
bool DalvikAssembler::decode1A(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_s(view, instruction, "const-string", DalvikOpcodes::ConstString); }

bool DalvikAssembler::decode1B(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode1C(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_t(view, instruction, "const-class", DalvikOpcodes::ConstClass); }
bool DalvikAssembler::decode1D(BufferView& view, const InstructionPtr &instruction) { return decodeOp1(view, instruction, "monitor-enter", DalvikOpcodes::MonitorEnter); }
bool DalvikAssembler::decode1E(BufferView& view, const InstructionPtr &instruction) { return decodeOp0(view, instruction, "monitor-exit", DalvikOpcodes::MonitorExit); }
bool DalvikAssembler::decode1F(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_t(view, instruction, "check-cast", DalvikOpcodes::CheckCast); }
bool DalvikAssembler::decode20(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_t(view, instruction, "instance-of", DalvikOpcodes::InstanceOf); }
bool DalvikAssembler::decode21(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "array-length", DalvikOpcodes::ArrayLength); }
bool DalvikAssembler::decode22(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_t(view, instruction, "new-instance", DalvikOpcodes::NewInstance); }
bool DalvikAssembler::decode23(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_t(view, instruction, "new-array", DalvikOpcodes::NewArray); }

bool DalvikAssembler::decode24(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode25(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode26(BufferView& view, const InstructionPtr &instruction)
{
    decodeOp2_imm32(view, instruction, "fill-array-data", DalvikOpcodes::FillArrayData);

    Operand* op = instruction->op(1);
    op->u_value = instruction->address + (op->u_value * sizeof(u16)); // Offset in 16-bit code units
    return true;
}

bool DalvikAssembler::decode27(BufferView& view, const InstructionPtr &instruction) { return decodeOp1(view, instruction, "throw-vx", DalvikOpcodes::Throw); }

bool DalvikAssembler::decode28(BufferView& view, const InstructionPtr &instruction)
{
    instruction->mnemonic = "goto";
    instruction->id = DalvikOpcodes::Goto;
    instruction->type = InstructionTypes::Jump;
    instruction->size = sizeof(u16);
    instruction->imm(instruction->address + (static_cast<s8>(*view) * sizeof(u16)));
    instruction->targetOp(0);
    return true;
}

bool DalvikAssembler::decode29(BufferView& view, const InstructionPtr &instruction)
{
    instruction->mnemonic = "goto/16";
    instruction->id = DalvikOpcodes::Goto_16;
    instruction->type = InstructionTypes::Jump;
    instruction->size = sizeof(u16) * 2;
    instruction->imm(instruction->address + (static_cast<s16>(view) * sizeof(u16)));
    instruction->targetOp(0);
    return true;
}

bool DalvikAssembler::decode2A(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode2B(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode2C(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode2D(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "cmpl-float", DalvikOpcodes::CmplFloat); }
bool DalvikAssembler::decode2E(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "cmpg-float", DalvikOpcodes::CmpgFloat); }
bool DalvikAssembler::decode2F(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "cmpl-double", DalvikOpcodes::CmplDouble); }
bool DalvikAssembler::decode30(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "cmpg-double", DalvikOpcodes::CmpgDouble); }
bool DalvikAssembler::decode31(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "cmp-long", DalvikOpcodes::CmpLong); }
bool DalvikAssembler::decode32(BufferView& view, const InstructionPtr &instruction) { return decodeIfOp3(view, instruction, "eq", DalvikOpcodes::IfEq); }
bool DalvikAssembler::decode33(BufferView& view, const InstructionPtr &instruction) { return decodeIfOp3(view, instruction, "ne", DalvikOpcodes::IfNe); }
bool DalvikAssembler::decode34(BufferView& view, const InstructionPtr &instruction) { return decodeIfOp3(view, instruction, "lt", DalvikOpcodes::IfLt); }
bool DalvikAssembler::decode35(BufferView& view, const InstructionPtr &instruction) { return decodeIfOp3(view, instruction, "ge", DalvikOpcodes::IfGe); }
bool DalvikAssembler::decode36(BufferView& view, const InstructionPtr &instruction) { return decodeIfOp3(view, instruction, "gt", DalvikOpcodes::IfGt); }
bool DalvikAssembler::decode37(BufferView& view, const InstructionPtr &instruction) { return decodeIfOp3(view, instruction, "le", DalvikOpcodes::IfLe); }
bool DalvikAssembler::decode38(BufferView& view, const InstructionPtr &instruction) { return decodeIfOp3(view, instruction, "eqz", DalvikOpcodes::IfEqz); }
bool DalvikAssembler::decode39(BufferView& view, const InstructionPtr &instruction) { return decodeIfOp2(view, instruction, "nez", DalvikOpcodes::IfNez); }
bool DalvikAssembler::decode3A(BufferView& view, const InstructionPtr &instruction) { return decodeIfOp2(view, instruction, "ltz", DalvikOpcodes::IfLtz); }
bool DalvikAssembler::decode3B(BufferView& view, const InstructionPtr &instruction) { return decodeIfOp2(view, instruction, "gez", DalvikOpcodes::IfGez); }
bool DalvikAssembler::decode3C(BufferView& view, const InstructionPtr &instruction) { return decodeIfOp2(view, instruction, "gtz", DalvikOpcodes::IfGtz); }
bool DalvikAssembler::decode3D(BufferView& view, const InstructionPtr &instruction) { return decodeIfOp2(view, instruction, "lez", DalvikOpcodes::IfLez); }

bool DalvikAssembler::decode3E(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode3F(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode40(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode41(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode42(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode43(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode44(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aget", DalvikOpcodes::Aget, InstructionTypes::Load); }
bool DalvikAssembler::decode45(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aget-wide", DalvikOpcodes::AgetWide, InstructionTypes::Load); }
bool DalvikAssembler::decode46(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aget-object", DalvikOpcodes::AgetObject, InstructionTypes::Load); }
bool DalvikAssembler::decode47(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aget-boolean", DalvikOpcodes::AgetBoolean, InstructionTypes::Load); }
bool DalvikAssembler::decode48(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aget-byte", DalvikOpcodes::AgetByte, InstructionTypes::Load); }
bool DalvikAssembler::decode49(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aget-char", DalvikOpcodes::AgetChar, InstructionTypes::Load); }
bool DalvikAssembler::decode4A(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aget-short", DalvikOpcodes::AgetShort, InstructionTypes::Load); }
bool DalvikAssembler::decode4B(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aput", DalvikOpcodes::Aput, InstructionTypes::Store); }
bool DalvikAssembler::decode4C(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aput-wide", DalvikOpcodes::AputWide, InstructionTypes::Store); }
bool DalvikAssembler::decode4D(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aput-object", DalvikOpcodes::AputObject, InstructionTypes::Store); }
bool DalvikAssembler::decode4E(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aput-boolean", DalvikOpcodes::AputBoolean, InstructionTypes::Store); }
bool DalvikAssembler::decode4F(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aput-byte", DalvikOpcodes::AputByte, InstructionTypes::Store); }
bool DalvikAssembler::decode50(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aput-char", DalvikOpcodes::AputChar, InstructionTypes::Store); }
bool DalvikAssembler::decode51(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "aput-short", DalvikOpcodes::AputShort, InstructionTypes::Store); }
bool DalvikAssembler::decode52(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iget", DalvikOpcodes::Iget, InstructionTypes::Load); }
bool DalvikAssembler::decode53(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iget-wide", DalvikOpcodes::IgetWide, InstructionTypes::Load); }
bool DalvikAssembler::decode54(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iget-object", DalvikOpcodes::IgetObject, InstructionTypes::Load); }
bool DalvikAssembler::decode55(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iget-boolean", DalvikOpcodes::IgetBoolean, InstructionTypes::Load); }
bool DalvikAssembler::decode56(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iget-byte", DalvikOpcodes::IgetByte, InstructionTypes::Load); }
bool DalvikAssembler::decode57(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iget-char", DalvikOpcodes::IgetChar, InstructionTypes::Load); }
bool DalvikAssembler::decode58(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iget-short", DalvikOpcodes::IgetShort, InstructionTypes::Load); }
bool DalvikAssembler::decode59(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iput", DalvikOpcodes::Iput, InstructionTypes::Store); }
bool DalvikAssembler::decode5A(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iput-wide", DalvikOpcodes::IputWide, InstructionTypes::Store); }
bool DalvikAssembler::decode5B(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iput-object", DalvikOpcodes::IputObject, InstructionTypes::Store); }
bool DalvikAssembler::decode5C(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iput-boolean", DalvikOpcodes::IputBoolean, InstructionTypes::Store); }
bool DalvikAssembler::decode5D(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iput-byte", DalvikOpcodes::IputByte, InstructionTypes::Store); }
bool DalvikAssembler::decode5E(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iput-char", DalvikOpcodes::IputChar, InstructionTypes::Store); }
bool DalvikAssembler::decode5F(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_f(view, instruction, "iput-short", DalvikOpcodes::IputShort, InstructionTypes::Store); }
bool DalvikAssembler::decode60(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sget", DalvikOpcodes::Sget, InstructionTypes::Load); }
bool DalvikAssembler::decode61(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sget-wide", DalvikOpcodes::SgetWide, InstructionTypes::Load); }
bool DalvikAssembler::decode62(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sget-object", DalvikOpcodes::SgetObject, InstructionTypes::Load); }
bool DalvikAssembler::decode63(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sget-boolean", DalvikOpcodes::SgetBoolean, InstructionTypes::Load); }
bool DalvikAssembler::decode64(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sget-byte", DalvikOpcodes::SgetByte, InstructionTypes::Load); }
bool DalvikAssembler::decode65(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sget-char", DalvikOpcodes::SgetChar, InstructionTypes::Load); }
bool DalvikAssembler::decode66(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sget-short", DalvikOpcodes::SgetShort, InstructionTypes::Load); }
bool DalvikAssembler::decode67(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sput", DalvikOpcodes::Sput, InstructionTypes::Store); }
bool DalvikAssembler::decode68(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sput-wide", DalvikOpcodes::SputWide, InstructionTypes::Store); }
bool DalvikAssembler::decode69(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sput-object", DalvikOpcodes::SputObject, InstructionTypes::Store); }
bool DalvikAssembler::decode6A(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sput-boolean", DalvikOpcodes::SputBoolean, InstructionTypes::Store); }
bool DalvikAssembler::decode6B(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sput-byte", DalvikOpcodes::SputByte, InstructionTypes::Store); }
bool DalvikAssembler::decode6C(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sput-char", DalvikOpcodes::SputChar, InstructionTypes::Store); }
bool DalvikAssembler::decode6D(BufferView& view, const InstructionPtr &instruction) { return decodeOp2_f(view, instruction, "sput-short", DalvikOpcodes::SputShort, InstructionTypes::Store); }
bool DalvikAssembler::decode6E(BufferView& view, const InstructionPtr &instruction) { return decodeInvoke(view, instruction, "virtual", DalvikOpcodes::InvokeVirtual); }
bool DalvikAssembler::decode6F(BufferView& view, const InstructionPtr &instruction) { return decodeInvoke(view, instruction, "super", DalvikOpcodes::InvokeSuper); }
bool DalvikAssembler::decode70(BufferView& view, const InstructionPtr &instruction) { return decodeInvoke(view, instruction, "direct", DalvikOpcodes::InvokeDirect); }
bool DalvikAssembler::decode71(BufferView& view, const InstructionPtr &instruction) { return decodeInvoke(view, instruction, "static", DalvikOpcodes::InvokeStatic); }
bool DalvikAssembler::decode72(BufferView& view, const InstructionPtr &instruction) { return decodeInvoke(view, instruction, "interface", DalvikOpcodes::InvokeInterface); }

bool DalvikAssembler::decode73(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode74(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode75(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode76(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode77(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode78(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode79(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode7A(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode7B(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "neg-int", DalvikOpcodes::NegInt); }

bool DalvikAssembler::decode7C(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode7D(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "neg-long", DalvikOpcodes::NegLong); }

bool DalvikAssembler::decode7E(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decode7F(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "neg-float", DalvikOpcodes::NegFloat); }
bool DalvikAssembler::decode80(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "neg-double", DalvikOpcodes::NegDouble); }
bool DalvikAssembler::decode81(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "int-to-long", DalvikOpcodes::IntToLong); }
bool DalvikAssembler::decode82(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "int-to-float", DalvikOpcodes::IntToFloat); }
bool DalvikAssembler::decode83(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "int-to-double", DalvikOpcodes::IntToDouble); }
bool DalvikAssembler::decode84(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "long-to-int", DalvikOpcodes::LongToInt); }
bool DalvikAssembler::decode85(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "long-to-float", DalvikOpcodes::LongToFloat); }
bool DalvikAssembler::decode86(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "long-to-double", DalvikOpcodes::LongToDouble); }
bool DalvikAssembler::decode87(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "float-to-int", DalvikOpcodes::FloatToInt); }
bool DalvikAssembler::decode88(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "float-to-long", DalvikOpcodes::FloatToLong); }
bool DalvikAssembler::decode89(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "float-to-double", DalvikOpcodes::FloatToDouble); }
bool DalvikAssembler::decode8A(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "double-to-int", DalvikOpcodes::DoubleToInt); }
bool DalvikAssembler::decode8B(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "double-to-long", DalvikOpcodes::DoubleToLong); }
bool DalvikAssembler::decode8C(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "double-to-float", DalvikOpcodes::DoubleToFloat); }
bool DalvikAssembler::decode8D(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "int-to-byte", DalvikOpcodes::IntToByte); }
bool DalvikAssembler::decode8E(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "int-to-char", DalvikOpcodes::IntToChar); }
bool DalvikAssembler::decode8F(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "int-to-short", DalvikOpcodes::IntToShort); }
bool DalvikAssembler::decode90(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "add-int", DalvikOpcodes::AddInt); }
bool DalvikAssembler::decode91(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "sub-int", DalvikOpcodes::SubInt); }
bool DalvikAssembler::decode92(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "mul-int", DalvikOpcodes::MulInt); }
bool DalvikAssembler::decode93(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "div-int", DalvikOpcodes::DivInt); }
bool DalvikAssembler::decode94(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "rem-int", DalvikOpcodes::RemInt); }
bool DalvikAssembler::decode95(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "and-int", DalvikOpcodes::AndInt); }
bool DalvikAssembler::decode96(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "or-int", DalvikOpcodes::OrInt); }
bool DalvikAssembler::decode97(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "xor-int", DalvikOpcodes::XorInt); }
bool DalvikAssembler::decode98(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "shl-int", DalvikOpcodes::ShlInt); }
bool DalvikAssembler::decode99(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "shr-int", DalvikOpcodes::ShrInt); }
bool DalvikAssembler::decode9A(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "ushr-int", DalvikOpcodes::UshrInt); }
bool DalvikAssembler::decode9B(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "add-long", DalvikOpcodes::AddLong); }
bool DalvikAssembler::decode9C(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "sub-long", DalvikOpcodes::SubLong); }
bool DalvikAssembler::decode9D(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "mul-long", DalvikOpcodes::MulLong); }
bool DalvikAssembler::decode9E(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "div-long", DalvikOpcodes::DivLong); }
bool DalvikAssembler::decode9F(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "rem-long", DalvikOpcodes::RemLong); }
bool DalvikAssembler::decodeA0(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "and-long", DalvikOpcodes::AndLong); }
bool DalvikAssembler::decodeA1(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "or-long", DalvikOpcodes::OrLong); }
bool DalvikAssembler::decodeA2(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "xor-long", DalvikOpcodes::XorLong); }
bool DalvikAssembler::decodeA3(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "shl-long", DalvikOpcodes::ShlLong); }
bool DalvikAssembler::decodeA4(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "shr-long", DalvikOpcodes::ShrLong); }
bool DalvikAssembler::decodeA5(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "ushr-long", DalvikOpcodes::UshrLong); }
bool DalvikAssembler::decodeA6(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "add-float", DalvikOpcodes::AddFloat); }
bool DalvikAssembler::decodeA7(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "sub-float", DalvikOpcodes::SubFloat); }
bool DalvikAssembler::decodeA8(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "mul-float", DalvikOpcodes::MulFloat); }
bool DalvikAssembler::decodeA9(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "div-float", DalvikOpcodes::DivFloat); }
bool DalvikAssembler::decodeAA(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "rem-float", DalvikOpcodes::RemFloat); }
bool DalvikAssembler::decodeAB(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "add-double", DalvikOpcodes::AddDouble); }
bool DalvikAssembler::decodeAC(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "sub-double", DalvikOpcodes::SubDouble); }
bool DalvikAssembler::decodeAD(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "mul-double", DalvikOpcodes::MulDouble); }
bool DalvikAssembler::decodeAE(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "div-double", DalvikOpcodes::DivDouble); }
bool DalvikAssembler::decodeAF(BufferView& view, const InstructionPtr &instruction) { return decodeOp3(view, instruction, "rem-double", DalvikOpcodes::RemDouble); }
bool DalvikAssembler::decodeB0(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "add-int/2addr", DalvikOpcodes::AddInt_2Addr); }
bool DalvikAssembler::decodeB1(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "sub-int/2addr", DalvikOpcodes::SubInt_2Addr); }
bool DalvikAssembler::decodeB2(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "mul-int/2addr", DalvikOpcodes::MulInt_2Addr); }
bool DalvikAssembler::decodeB3(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "div-int/2addr", DalvikOpcodes::DivInt_2Addr); }
bool DalvikAssembler::decodeB4(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "rem-int/2addr", DalvikOpcodes::RemInt_2Addr); }
bool DalvikAssembler::decodeB5(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "and-int/2addr", DalvikOpcodes::AndInt_2Addr); }
bool DalvikAssembler::decodeB6(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "or-int/2addr", DalvikOpcodes::OrInt_2Addr); }
bool DalvikAssembler::decodeB7(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "xor-int/2addr", DalvikOpcodes::XorInt_2Addr); }
bool DalvikAssembler::decodeB8(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "shl-int/2addr", DalvikOpcodes::ShlInt_2Addr); }
bool DalvikAssembler::decodeB9(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "shr-int/2addr", DalvikOpcodes::ShrInt_2Addr); }
bool DalvikAssembler::decodeBA(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "ushr-int/2addr", DalvikOpcodes::UshrInt_2Addr); }
bool DalvikAssembler::decodeBB(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "add-long/2addr", DalvikOpcodes::AddLong_2Addr); }
bool DalvikAssembler::decodeBC(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "sub-long/2addr", DalvikOpcodes::SubLong_2Addr); }
bool DalvikAssembler::decodeBD(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "mul-long/2addr", DalvikOpcodes::MulLong_2Addr); }
bool DalvikAssembler::decodeBE(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "div-long/2addr", DalvikOpcodes::DivLong_2Addr); }
bool DalvikAssembler::decodeBF(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "rem-long/2addr", DalvikOpcodes::RemLong_2Addr); }
bool DalvikAssembler::decodeC0(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "and-long/2addr", DalvikOpcodes::AndLong_2Addr); }
bool DalvikAssembler::decodeC1(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "or-long/2addr", DalvikOpcodes::OrLong_2Addr); }
bool DalvikAssembler::decodeC2(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "xor-long/2addr", DalvikOpcodes::XorLong_2Addr); }
bool DalvikAssembler::decodeC3(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "shl-long/2addr", DalvikOpcodes::ShlLong_2Addr); }
bool DalvikAssembler::decodeC4(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "shr-long/2addr", DalvikOpcodes::ShrLong_2Addr); }
bool DalvikAssembler::decodeC5(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "ushr-long/2addr", DalvikOpcodes::UshrLong_2Addr); }
bool DalvikAssembler::decodeC6(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "add-float/2addr", DalvikOpcodes::AddFloat_2Addr); }
bool DalvikAssembler::decodeC7(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "sub-float/2addr", DalvikOpcodes::SubFloat_2Addr); }
bool DalvikAssembler::decodeC8(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "mul-float/2addr", DalvikOpcodes::MulFloat_2Addr); }
bool DalvikAssembler::decodeC9(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "div-float/2addr", DalvikOpcodes::DivFloat_2Addr); }
bool DalvikAssembler::decodeCA(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "rem-float/2addr", DalvikOpcodes::RemFloat_2Addr); }
bool DalvikAssembler::decodeCB(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "add-double/2addr", DalvikOpcodes::AddDouble_2Addr); }
bool DalvikAssembler::decodeCC(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "sub-double/2addr", DalvikOpcodes::SubDouble_2Addr); }
bool DalvikAssembler::decodeCD(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "mul-double/2addr", DalvikOpcodes::MulDouble_2Addr); }
bool DalvikAssembler::decodeCE(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "div-double/2addr", DalvikOpcodes::DivDouble_2Addr); }
bool DalvikAssembler::decodeCF(BufferView& view, const InstructionPtr &instruction) { return decodeOp2(view, instruction, "rem-double/2addr", DalvikOpcodes::RemDouble_2Addr); }
bool DalvikAssembler::decodeD0(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm16(view, instruction, "add-int/lit16", DalvikOpcodes::AddIntLit16); }
bool DalvikAssembler::decodeD1(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm16(view, instruction, "sub-int/lit16", DalvikOpcodes::SubIntLit16); }
bool DalvikAssembler::decodeD2(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm16(view, instruction, "mul-int/lit16", DalvikOpcodes::MulIntLit16); }
bool DalvikAssembler::decodeD3(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm16(view, instruction, "div-int/lit16", DalvikOpcodes::DivIntLit16); }
bool DalvikAssembler::decodeD4(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm16(view, instruction, "rem-int/lit16", DalvikOpcodes::RemIntLit16); }
bool DalvikAssembler::decodeD5(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm16(view, instruction, "and-int/lit16", DalvikOpcodes::AndIntLit16); }
bool DalvikAssembler::decodeD6(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm16(view, instruction, "or-int/lit16", DalvikOpcodes::OrIntLit16); }
bool DalvikAssembler::decodeD7(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm16(view, instruction, "xor-int/lit16", DalvikOpcodes::XorIntLit16); }
bool DalvikAssembler::decodeD8(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm8(view, instruction, "add-int/lit8", DalvikOpcodes::AddIntLit8); }
bool DalvikAssembler::decodeD9(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm8(view, instruction, "sub-int/lit8", DalvikOpcodes::SubIntLit8); }
bool DalvikAssembler::decodeDA(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm8(view, instruction, "mul-int/lit8", DalvikOpcodes::MulIntLit8); }
bool DalvikAssembler::decodeDB(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm8(view, instruction, "div-int/lit8", DalvikOpcodes::DivIntLit8); }
bool DalvikAssembler::decodeDC(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm8(view, instruction, "rem-int/lit8", DalvikOpcodes::RemIntLit8); }
bool DalvikAssembler::decodeDD(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm8(view, instruction, "and-int/lit8", DalvikOpcodes::AndIntLit8); }
bool DalvikAssembler::decodeDE(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm8(view, instruction, "or-int/lit8", DalvikOpcodes::OrIntLit8); }
bool DalvikAssembler::decodeDF(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm8(view, instruction, "xor-int/lit8", DalvikOpcodes::XorIntLit8); }
bool DalvikAssembler::decodeE0(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm8(view, instruction, "shl-int/lit8", DalvikOpcodes::ShlIntLit8); }
bool DalvikAssembler::decodeE1(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm8(view, instruction, "shr-int/lit8", DalvikOpcodes::ShrIntLit8); }
bool DalvikAssembler::decodeE2(BufferView& view, const InstructionPtr &instruction) { return decodeOp3_imm8(view, instruction, "ushr-int/lit8", DalvikOpcodes::UshrIntLit8); }

bool DalvikAssembler::decodeE3(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeE4(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeE5(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeE6(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeE7(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeE8(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeE9(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeEA(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeEB(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeEC(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeED(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeEE(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeEF(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF0(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF1(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF2(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF3(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF4(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF5(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF6(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF7(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF8(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeF9(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeFA(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeFB(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeFC(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeFD(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeFE(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

bool DalvikAssembler::decodeFF(BufferView& view, const InstructionPtr &instruction)
{
    return false;
}

} // namespace REDasm
