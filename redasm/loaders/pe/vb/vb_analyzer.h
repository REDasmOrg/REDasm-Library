#pragma once

#include "../pe_analyzer.h"
#include "vb_header.h"

namespace REDasm {

class VBAnalyzer : public PEAnalyzer
{
    public:
        VBAnalyzer(const PEClassifier* classifier, DisassemblerAPI* disassembler);
        virtual void analyze();

    private:
        void disassembleTrampoline(address_t eventva, const std::string &name);
        void decompileObject(const VBPublicObjectDescriptor& pubobjdescr);
        bool decompile(address_t thunrtdata);

    private:
        const LoaderPlugin* m_loader;
        VBHeader* m_vbheader;
        VBProjectInfo* m_vbprojinfo;
        VBObjectTable* m_vbobjtable;
        VBObjectTreeInfo* m_vbobjtreeinfo;
        VBPublicObjectDescriptor* m_vbpubobjdescr;
};

} // namespace REDasm
