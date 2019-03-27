#include "vb_analyzer.h"
#include "vb_components.h"
#include "../pe.h"
#include "../../../support/symbolize.h"

#define HAS_OPTIONAL_INFO(objdescr, objinfo) (objdescr.lpObjectInfo + sizeof(VBObjectInfo) != objinfo->base.lpConstants)
#define VB_METHODNAME(pubobj, control, method) (pubobj + "_" + control + "_" + method)

namespace REDasm {

VBAnalyzer::VBAnalyzer(const PEClassifier *classifier, DisassemblerAPI *disassembler): PEAnalyzer(classifier, disassembler)
{
    m_loader = nullptr;
    m_vbheader = nullptr;
    m_vbprojinfo = nullptr;
    m_vbobjtable = nullptr;
    m_vbobjtreeinfo = nullptr;
    m_vbpubobjdescr = nullptr;
}

void VBAnalyzer::analyze()
{
    InstructionPtr instruction = m_document->entryInstruction();

    if(!instruction->is(InstructionTypes::Push) || (instruction->operands.size() != 1))
        return;

    if(!instruction->op(0)->is(OperandTypes::Immediate))
        return;

    address_t thunrtdata = instruction->op(0)->u_value;

    if(!m_document->segment(thunrtdata) || !m_document->advance(instruction) || !instruction->is(InstructionTypes::Call))
        return;

    instruction->type |= InstructionTypes::Stop;
    m_document->update(instruction);

    if(!this->decompile(thunrtdata))
        return;

    PEAnalyzer::analyze();
}

void VBAnalyzer::disassembleTrampoline(address_t eventva, const std::string& name)
{
    if(!eventva)
        return;

    InstructionPtr instruction = m_disassembler->disassembleInstruction(eventva); // Disassemble trampoline

    if(instruction->mnemonic == "sub")
    {
        this->disassembleTrampoline(instruction->endAddress(), name); // Jump follows...
        return;
    }

    REDasm::statusAddress("Decoding " + name, eventva);

    if(instruction->is(InstructionTypes::Branch))
    {
        const Operand* op = instruction->target();

        if(!op)
            return;

        m_disassembler->disassemble(op->u_value);
        m_document->lock(op->u_value, name, SymbolTypes::Function);
    }
}

void VBAnalyzer::decompileObject(const VBPublicObjectDescriptor &pubobjdescr)
{
    if(!pubobjdescr.lpObjectInfo)
        return;

    VBObjectInfoOptional* objinfo = m_loader->addrpointer<VBObjectInfoOptional>(pubobjdescr.lpObjectInfo);

    // if lpConstants points to the address after it,
    // there's no optional object information
    if(!HAS_OPTIONAL_INFO(pubobjdescr, objinfo) || !objinfo->lpControls)
        return;

    std::string pubobjname = m_loader->addrpointer<const char>(pubobjdescr.lpszObjectName);
    VBControlInfo* ctrlinfo = m_loader->addrpointer<VBControlInfo>(objinfo->lpControls);

    for(size_t i = 0; i < objinfo->dwControlCount; i++)
    {
        const VBControlInfo& ctrl = ctrlinfo[i];
        const VBComponents::Component* component = VBComponents::get(m_loader->addrpointer<GUID>(ctrl.lpGuid));

        if(!component)
            continue;

        VBEventInfo* eventinfo = m_loader->addrpointer<VBEventInfo>(ctrl.lpEventInfo);
        std::string componentname = m_loader->addrpointer<const char>(ctrl.lpszName);
        u32* events = &eventinfo->lpEvents[0];

        for(size_t j = 0; j < component->events.size(); j++)
            this->disassembleTrampoline(events[j], VB_METHODNAME(pubobjname, componentname, component->events[j]));
    }
}

bool VBAnalyzer::decompile(address_t thunrtdata)
{
    m_loader = m_disassembler->loader();
    m_vbheader = m_loader->addrpointer<VBHeader>(thunrtdata);

    if(std::strncmp(m_vbheader->szVbMagic, "VB5!", VB_SIGNATURE_SIZE))
        return false;

    m_vbprojinfo = m_loader->addrpointer<VBProjectInfo>(m_vbheader->lpProjectData);
    m_vbobjtable = m_loader->addrpointer<VBObjectTable>(m_vbprojinfo->lpObjectTable);
    m_vbobjtreeinfo = m_loader->addrpointer<VBObjectTreeInfo>(m_vbobjtable->lpObjectTreeInfo);
    m_vbpubobjdescr = m_loader->addrpointer<VBPublicObjectDescriptor>(m_vbobjtable->lpPubObjArray);

    REDASM_SYMBOLIZE(VBHeader, m_disassembler, thunrtdata);
    REDASM_SYMBOLIZE(VBProjectInfo, m_disassembler, m_vbheader->lpProjectData);
    REDASM_SYMBOLIZE(VBObjectTable, m_disassembler, m_vbprojinfo->lpObjectTable);
    REDASM_SYMBOLIZE(VBObjectTreeInfo, m_disassembler, m_vbobjtable->lpObjectTreeInfo);
    REDASM_SYMBOLIZE(VBPublicObjectDescriptor, m_disassembler, m_vbobjtable->lpPubObjArray);

    for(size_t i = 0; i < m_vbobjtable->wTotalObjects; i++)
        this->decompileObject(m_vbpubobjdescr[i]);

    return true;
}

} // namespace REDasm
