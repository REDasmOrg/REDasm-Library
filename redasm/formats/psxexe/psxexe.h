#ifndef PSXEXE_H
#define PSXEXE_H

#include "../../plugins/plugins.h"

#define PSXEXE_SIGNATURE_SIZE 8

namespace REDasm {

struct PsxExeHeader
{
    char id[PSXEXE_SIGNATURE_SIZE];
    u32le text, data;
    u32le pc0, gp0;
    u32le t_addr, t_size;
    u32le d_addr, d_size;
    u32le b_addr, b_size;
    u32le s_addr, s_size;
    u32le SavedSP, SavedFP, SavedGP, SavedRA, SavedS0;
};

class PsxExeFormat: public FormatPluginT<PsxExeHeader>
{
    DEFINE_FORMAT_PLUGIN_TEST(PsxExeHeader)

    public:
        PsxExeFormat(Buffer& buffer);
        virtual std::string name() const;
        virtual std::string assembler() const;
        virtual u32 bits() const;
        virtual Analyzer* createAnalyzer(DisassemblerAPI *disassembler, const SignatureFiles &signatures) const;
        virtual void load();
};

DECLARE_FORMAT_PLUGIN(PsxExeFormat, psxexe)

}

#endif // PSXEXE_H
