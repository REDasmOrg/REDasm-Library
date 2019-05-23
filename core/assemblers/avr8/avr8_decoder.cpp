#include "avr8_decoder.h"
#include "avr8_opcodes.h"

#define DEFINE_AVR8_INSTRUCTION(mask, id, mnemonic, size, operands) m_avrinstructions.push_front({mask, id, mnemonic, size, UNPAREN(operands) })

namespace REDasm {

std::forward_list<AVR8Instruction> AVR8Decoder::m_avrinstructions;

const AVR8Instruction *AVR8Decoder::get(u16 opcode)
{
    AVR8Decoder::initializeInstructions();

    for(const auto& avrinstruction : m_avrinstructions)
    {
        u16 mask = opcode;

        for(const auto& avrop : avrinstruction.operands)
            mask &= ~avrop.mask;

        if(mask == avrinstruction.mask)
            return &avrinstruction;
    }

    return nullptr;
}

void AVR8Decoder::initializeInstructions()
{
    if(!m_avrinstructions.empty())
        return;

    DEFINE_AVR8_INSTRUCTION(0x9598, AVR8Opcodes::Break_9598, "break", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9488, AVR8Opcodes::Clc_9488, "clc", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x94d8, AVR8Opcodes::Clh_94d8, "clh", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x94f8, AVR8Opcodes::Cli_94f8, "cli", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x94a8, AVR8Opcodes::Cln_94a8, "cln", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x94c8, AVR8Opcodes::Cls_94c8, "cls", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x94e8, AVR8Opcodes::Clt_94e8, "clt", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x94b8, AVR8Opcodes::Clv_94b8, "clv", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9498, AVR8Opcodes::Clz_9498, "clz", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9519, AVR8Opcodes::Eicall_9519, "eicall", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9419, AVR8Opcodes::Eijmp_9419, "eijmp", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x95d8, AVR8Opcodes::Elpm_95d8, "elpm", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9509, AVR8Opcodes::Icall_9509, "icall", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9409, AVR8Opcodes::Ijmp_9409, "ijmp", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x95c8, AVR8Opcodes::Lpm_95c8, "lpm", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x0000, AVR8Opcodes::Nop_0000, "nop", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9508, AVR8Opcodes::Ret_9508, "ret", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9518, AVR8Opcodes::Reti_9518, "reti", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9408, AVR8Opcodes::Sec_9408, "sec", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9458, AVR8Opcodes::Seh_9458, "seh", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9478, AVR8Opcodes::Sei_9478, "sei", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9428, AVR8Opcodes::Sen_9428, "sen", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9448, AVR8Opcodes::Ses_9448, "ses", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9468, AVR8Opcodes::Set_9468, "set", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9438, AVR8Opcodes::Sev_9438, "sev", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9418, AVR8Opcodes::Sez_9418, "sez", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x9588, AVR8Opcodes::Sleep_9588, "sleep", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x95e8, AVR8Opcodes::Spm_95e8, "spm", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x95f8, AVR8Opcodes::Spm_95f8, "spm", 2, ({ { 0x0000, AVR8Operands::ZP } }));
    DEFINE_AVR8_INSTRUCTION(0x95a8, AVR8Opcodes::Wdr_95a8, "wdr", 2, ({  }));
    DEFINE_AVR8_INSTRUCTION(0x940b, AVR8Opcodes::Des_940b, "des", 2, ({ { 0x00f0, AVR8Operands::DESRound } }));
    DEFINE_AVR8_INSTRUCTION(0x9405, AVR8Opcodes::Asr_9405, "asr", 2, ({ { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x9488, AVR8Opcodes::Bclr_9488, "bclr", 2, ({ { 0x0070, AVR8Operands::Bit } }));
    DEFINE_AVR8_INSTRUCTION(0xf400, AVR8Opcodes::Brcc_f400, "brcc", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf000, AVR8Opcodes::Brcs_f000, "brcs", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf001, AVR8Opcodes::Breq_f001, "breq", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf404, AVR8Opcodes::Brge_f404, "brge", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf405, AVR8Opcodes::Brhc_f405, "brhc", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf005, AVR8Opcodes::Brhs_f005, "brhs", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf407, AVR8Opcodes::Brid_f407, "brid", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf007, AVR8Opcodes::Brie_f007, "brie", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf000, AVR8Opcodes::Brlo_f000, "brlo", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf004, AVR8Opcodes::Brlt_f004, "brlt", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf002, AVR8Opcodes::Brmi_f002, "brmi", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf401, AVR8Opcodes::Brne_f401, "brne", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf402, AVR8Opcodes::Brpl_f402, "brpl", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf400, AVR8Opcodes::Brsh_f400, "brsh", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf406, AVR8Opcodes::Brtc_f406, "brtc", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf006, AVR8Opcodes::Brts_f006, "brts", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf403, AVR8Opcodes::Brvc_f403, "brvc", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf003, AVR8Opcodes::Brvs_f003, "brvs", 2, ({ { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0x9408, AVR8Opcodes::Bset_9408, "bset", 2, ({ { 0x0070, AVR8Operands::Bit } }));
    DEFINE_AVR8_INSTRUCTION(0x940e, AVR8Opcodes::Call_940e, "call", 4, ({ { 0x01f1, AVR8Operands::LongAbsoluteAddress } }));
    DEFINE_AVR8_INSTRUCTION(0x9400, AVR8Opcodes::Com_9400, "com", 2, ({ { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x940a, AVR8Opcodes::Dec_940a, "dec", 2, ({ { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x9403, AVR8Opcodes::Inc_9403, "inc", 2, ({ { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x940c, AVR8Opcodes::Jmp_940c, "jmp", 4, ({ { 0x01f1, AVR8Operands::LongAbsoluteAddress } }));
    DEFINE_AVR8_INSTRUCTION(0x9004, AVR8Opcodes::Lpm_9004, "lpm", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::Z } }));
    DEFINE_AVR8_INSTRUCTION(0x9005, AVR8Opcodes::Lpm_9005, "lpm", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::ZP } }));
    DEFINE_AVR8_INSTRUCTION(0x9406, AVR8Opcodes::Lsr_9406, "lsr", 2, ({ { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x9401, AVR8Opcodes::Neg_9401, "neg", 2, ({ { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x900f, AVR8Opcodes::Pop_900f, "pop", 2, ({ { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x9204, AVR8Opcodes::Xch_9204, "xch", 2, ({ { 0x0000, AVR8Operands::Z }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x9205, AVR8Opcodes::Las_9205, "las", 2, ({ { 0x0000, AVR8Operands::Z }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x9206, AVR8Opcodes::Lac_9206, "lac", 2, ({ { 0x0000, AVR8Operands::Z }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x9207, AVR8Opcodes::Lat_9207, "lat", 2, ({ { 0x0000, AVR8Operands::Z }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x920f, AVR8Opcodes::Push_920f, "push", 2, ({ { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0xd000, AVR8Opcodes::Rcall_d000, "rcall", 2, ({ { 0x0fff, AVR8Operands::RelativeAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xc000, AVR8Opcodes::Rjmp_c000, "rjmp", 2, ({ { 0x0fff, AVR8Operands::RelativeAddress } }));
    DEFINE_AVR8_INSTRUCTION(0x9407, AVR8Opcodes::Ror_9407, "ror", 2, ({ { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0xef0f, AVR8Opcodes::Ser_ef0f, "ser", 2, ({ { 0x00f0, AVR8Operands::RegisterStartR16 } }));
    DEFINE_AVR8_INSTRUCTION(0x9402, AVR8Opcodes::Swap_9402, "swap", 2, ({ { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x1c00, AVR8Opcodes::Adc_1c00, "adc", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x020f, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x0c00, AVR8Opcodes::Add_0c00, "add", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x020f, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x9600, AVR8Opcodes::Adiw_9600, "adiw", 2, ({ { 0x0030, AVR8Operands::RegisterEvenPairStartR24 }, { 0x00cf, AVR8Operands::Data } }));
    DEFINE_AVR8_INSTRUCTION(0x2000, AVR8Opcodes::And_2000, "and", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x020f, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x7000, AVR8Opcodes::Andi_7000, "andi", 2, ({ { 0x00f0, AVR8Operands::RegisterStartR16 }, { 0x0f0f, AVR8Operands::Data } }));
    DEFINE_AVR8_INSTRUCTION(0xf800, AVR8Opcodes::Bld_f800, "bld", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0007, AVR8Operands::Bit } }));
    DEFINE_AVR8_INSTRUCTION(0xf400, AVR8Opcodes::Brbc_f400, "brbc", 2, ({ { 0x0007, AVR8Operands::Bit }, { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xf000, AVR8Opcodes::Brbs_f000, "brbs", 2, ({ { 0x0007, AVR8Operands::Bit }, { 0x03f8, AVR8Operands::BranchAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xfa00, AVR8Opcodes::Bst_fa00, "bst", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0007, AVR8Operands::Bit } }));
    DEFINE_AVR8_INSTRUCTION(0x9800, AVR8Opcodes::Cbi_9800, "cbi", 2, ({ { 0x00f8, AVR8Operands::IORegister }, { 0x0007, AVR8Operands::Bit } }));
    DEFINE_AVR8_INSTRUCTION(0x1400, AVR8Opcodes::Cp_1400, "cp", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x020f, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x0400, AVR8Opcodes::Cpc_0400, "cpc", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x020f, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x3000, AVR8Opcodes::Cpi_3000, "cpi", 2, ({ { 0x00f0, AVR8Operands::RegisterStartR16 }, { 0x0f0f, AVR8Operands::Data } }));
    DEFINE_AVR8_INSTRUCTION(0x1000, AVR8Opcodes::Cpse_1000, "cpse", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x020f, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x9006, AVR8Opcodes::Elpm_9006, "elpm", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::Z } }));
    DEFINE_AVR8_INSTRUCTION(0x9007, AVR8Opcodes::Elpm_9007, "elpm", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::ZP } }));
    DEFINE_AVR8_INSTRUCTION(0x2400, AVR8Opcodes::Eor_2400, "eor", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x020f, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x0308, AVR8Opcodes::Fmul_0308, "fmul", 2, ({ { 0x0070, AVR8Operands::RegisterStartR16 }, { 0x0007, AVR8Operands::RegisterStartR16 } }));
    DEFINE_AVR8_INSTRUCTION(0x0380, AVR8Opcodes::Fmuls_0380, "fmuls", 2, ({ { 0x0070, AVR8Operands::RegisterStartR16 }, { 0x0007, AVR8Operands::RegisterStartR16 } }));
    DEFINE_AVR8_INSTRUCTION(0x0388, AVR8Opcodes::Fmulsu_0388, "fmulsu", 2, ({ { 0x0070, AVR8Operands::RegisterStartR16 }, { 0x0007, AVR8Operands::RegisterStartR16 } }));
    DEFINE_AVR8_INSTRUCTION(0xb000, AVR8Opcodes::In_b000, "in", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x060f, AVR8Operands::Data } }));
    DEFINE_AVR8_INSTRUCTION(0x900c, AVR8Opcodes::Ld_900c, "ld", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::X } }));
    DEFINE_AVR8_INSTRUCTION(0x900d, AVR8Opcodes::Ld_900d, "ld", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::XP } }));
    DEFINE_AVR8_INSTRUCTION(0x900e, AVR8Opcodes::Ld_900e, "ld", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::MX } }));
    DEFINE_AVR8_INSTRUCTION(0x8008, AVR8Opcodes::Ld_8008, "ld", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::Y } }));
    DEFINE_AVR8_INSTRUCTION(0x9009, AVR8Opcodes::Ld_9009, "ld", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::YP } }));
    DEFINE_AVR8_INSTRUCTION(0x900a, AVR8Opcodes::Ld_900a, "ld", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::MY } }));
    DEFINE_AVR8_INSTRUCTION(0x8000, AVR8Opcodes::Ld_8000, "ld", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::Z } }));
    DEFINE_AVR8_INSTRUCTION(0x9001, AVR8Opcodes::Ld_9001, "ld", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::ZP } }));
    DEFINE_AVR8_INSTRUCTION(0x9002, AVR8Opcodes::Ld_9002, "ld", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::MZ } }));
    DEFINE_AVR8_INSTRUCTION(0x8008, AVR8Opcodes::Ldd_8008, "ldd", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x2c07, AVR8Operands::YPQ } }));
    DEFINE_AVR8_INSTRUCTION(0x8000, AVR8Opcodes::Ldd_8000, "ldd", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x2c07, AVR8Operands::ZPQ } }));
    DEFINE_AVR8_INSTRUCTION(0xe000, AVR8Opcodes::Ldi_e000, "ldi", 2, ({ { 0x00f0, AVR8Operands::RegisterStartR16 }, { 0x0f0f, AVR8Operands::Data } }));
    DEFINE_AVR8_INSTRUCTION(0x9000, AVR8Opcodes::Lds_9000, "lds", 4, ({ { 0x01f0, AVR8Operands::Register }, { 0x0000, AVR8Operands::LongAbsoluteAddress } }));
    DEFINE_AVR8_INSTRUCTION(0xa000, AVR8Opcodes::Lds_a000, "lds", 2, ({ { 0x00f0, AVR8Operands::RegisterStartR16 }, { 0x070f, AVR8Operands::Data } }));
    DEFINE_AVR8_INSTRUCTION(0x2c00, AVR8Opcodes::Mov_2c00, "mov", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x020f, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x0100, AVR8Opcodes::Movw_0100, "movw", 2, ({ { 0x00f0, AVR8Operands::RegisterEvenPair }, { 0x000f, AVR8Operands::RegisterEvenPair } }));
    DEFINE_AVR8_INSTRUCTION(0x9c00, AVR8Opcodes::Mul_9c00, "mul", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x020f, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x0200, AVR8Opcodes::Muls_0200, "muls", 2, ({ { 0x00f0, AVR8Operands::RegisterStartR16 }, { 0x000f, AVR8Operands::RegisterStartR16 } }));
    DEFINE_AVR8_INSTRUCTION(0x0300, AVR8Opcodes::Mulsu_0300, "mulsu", 2, ({ { 0x0070, AVR8Operands::RegisterStartR16 }, { 0x0007, AVR8Operands::RegisterStartR16 } }));
    DEFINE_AVR8_INSTRUCTION(0x2800, AVR8Opcodes::Or_2800, "or", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x020f, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x6000, AVR8Opcodes::Ori_6000, "ori", 2, ({ { 0x00f0, AVR8Operands::RegisterStartR16 }, { 0x0f0f, AVR8Operands::Data } }));
    DEFINE_AVR8_INSTRUCTION(0xb800, AVR8Opcodes::Out_b800, "out", 2, ({ { 0x060f, AVR8Operands::IORegister }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x0800, AVR8Opcodes::Sbc_0800, "sbc", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x020f, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x4000, AVR8Opcodes::Sbci_4000, "sbci", 2, ({ { 0x00f0, AVR8Operands::RegisterStartR16 }, { 0x0f0f, AVR8Operands::Data } }));
    DEFINE_AVR8_INSTRUCTION(0x9a00, AVR8Opcodes::Sbi_9a00, "sbi", 2, ({ { 0x00f8, AVR8Operands::IORegister }, { 0x0007, AVR8Operands::Bit } }));
    DEFINE_AVR8_INSTRUCTION(0x9900, AVR8Opcodes::Sbic_9900, "sbic", 2, ({ { 0x00f8, AVR8Operands::IORegister }, { 0x0007, AVR8Operands::Bit } }));
    DEFINE_AVR8_INSTRUCTION(0x9b00, AVR8Opcodes::Sbis_9b00, "sbis", 2, ({ { 0x00f8, AVR8Operands::IORegister }, { 0x0007, AVR8Operands::Bit } }));
    DEFINE_AVR8_INSTRUCTION(0x9700, AVR8Opcodes::Sbiw_9700, "sbiw", 2, ({ { 0x0030, AVR8Operands::RegisterEvenPairStartR24 }, { 0x00cf, AVR8Operands::Data } }));
    DEFINE_AVR8_INSTRUCTION(0x6000, AVR8Opcodes::Sbr_6000, "sbr", 2, ({ { 0x00f0, AVR8Operands::RegisterStartR16 }, { 0x0f0f, AVR8Operands::Data } }));
    DEFINE_AVR8_INSTRUCTION(0xfc00, AVR8Opcodes::Sbrc_fc00, "sbrc", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0007, AVR8Operands::Bit } }));
    DEFINE_AVR8_INSTRUCTION(0xfe00, AVR8Opcodes::Sbrs_fe00, "sbrs", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x0007, AVR8Operands::Bit } }));
    DEFINE_AVR8_INSTRUCTION(0x920c, AVR8Opcodes::St_920c, "st", 2, ({ { 0x0000, AVR8Operands::X }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x920d, AVR8Opcodes::St_920d, "st", 2, ({ { 0x0000, AVR8Operands::XP }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x920e, AVR8Opcodes::St_920e, "st", 2, ({ { 0x0000, AVR8Operands::MX }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x8208, AVR8Opcodes::St_8208, "st", 2, ({ { 0x0000, AVR8Operands::Y }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x9209, AVR8Opcodes::St_9209, "st", 2, ({ { 0x0000, AVR8Operands::YP }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x920a, AVR8Opcodes::St_920a, "st", 2, ({ { 0x0000, AVR8Operands::MY }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x8200, AVR8Opcodes::St_8200, "st", 2, ({ { 0x0000, AVR8Operands::Z }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x9201, AVR8Opcodes::St_9201, "st", 2, ({ { 0x0000, AVR8Operands::ZP }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x9202, AVR8Opcodes::St_9202, "st", 2, ({ { 0x0000, AVR8Operands::MZ }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x8208, AVR8Opcodes::Std_8208, "std", 2, ({ { 0x2c07, AVR8Operands::YPQ }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x8200, AVR8Opcodes::Std_8200, "std", 2, ({ { 0x2c07, AVR8Operands::ZPQ }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x9200, AVR8Opcodes::Sts_9200, "sts", 4, ({ { 0x0000, AVR8Operands::LongAbsoluteAddress }, { 0x01f0, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0xa800, AVR8Opcodes::Sts_a800, "sts", 2, ({ { 0x00f0, AVR8Operands::RegisterStartR16 }, { 0x070f, AVR8Operands::Data } }));
    DEFINE_AVR8_INSTRUCTION(0x1800, AVR8Opcodes::Sub_1800, "sub", 2, ({ { 0x01f0, AVR8Operands::Register }, { 0x020f, AVR8Operands::Register } }));
    DEFINE_AVR8_INSTRUCTION(0x5000, AVR8Opcodes::Subi_5000, "subi", 2, ({ { 0x00f0, AVR8Operands::RegisterStartR16 }, { 0x0f0f, AVR8Operands::Data } }));
}

} // namespace REDasm
