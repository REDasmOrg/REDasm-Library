#ifndef PSXEXELOADER_H
#define PSXEXELOADER_H

#include "../../plugins/plugins.h"

#define PSXEXE_SIGNATURE_SIZE 8

namespace REDasm {

struct PsxExeHeader
{
    char id[PSXEXE_SIGNATURE_SIZE];
    u32 text, data;
    u32 pc0, gp0;
    u32 t_addr, t_size;
    u32 d_addr, d_size;
    u32 b_addr, b_size;
    u32 s_addr, s_size;
    u32 SavedSP, SavedFP, SavedGP, SavedRA, SavedS0;
};

class PsxExeLoader: public LoaderPluginT<PsxExeHeader>
{
    PLUGIN_NAME("PS-X Executable")
    DECLARE_LOADER_PLUGIN_TEST(PsxExeHeader)

    public:
        PsxExeLoader(AbstractBuffer* buffer);
        virtual std::string assembler() const;
        virtual u32 bits() const;
        virtual Analyzer* createAnalyzer(DisassemblerAPI *disassembler, const SignatureFiles &signatures) const;
        virtual void load();
};

DECLARE_LOADER_PLUGIN(PsxExeLoader, psxexe)

}

#endif // PSXEXELOADER_H
