#include "mscoff.h"
#include "../../support/coff/coff_symboltable.h"

namespace REDasm {

LOADER_PLUGIN_TEST(MSCOFFLoader, ImageArchiveHeader)
{
    if(strncmp(header->signature, MSCOFF_SIGNATURE, MSCOFF_SIGNATURE_SIZE))
        return false;

    if((header->first.name[0] != '/') || (std::stoi(header->first.size) <= 0))
        return false;

    return (header->first.endheader[0] == 0x60) && (header->first.endheader[1] == 0x0a);
}

MSCOFFLoader::MSCOFFLoader(AbstractBuffer *buffer): LoaderPluginT<ImageArchiveHeader>(buffer), m_firstlinkerhdr(nullptr), m_secondlinkerhdr(nullptr), m_longnameshdr(nullptr) { }
std::string MSCOFFLoader::assembler() const
{
    if(m_machines.empty())
        return std::string();

    u16 machine = *m_machines.begin();

    if(machine == IMAGE_FILE_MACHINE_I386)
        return "x86_32";
    if(machine == IMAGE_FILE_MACHINE_AMD64)
        return "x86_64";
    if(machine == IMAGE_FILE_MACHINE_ARM)
        return "arm";

    return std::string();
}

void MSCOFFLoader::load()
{
    this->readMemberHeaders();

    if(m_machines.size() == 1)
        return;

    REDasm::log("Invalid number of assemblers, expected 1, got " + std::to_string(m_machines.size()));
    m_document = this->createDocument(); // Reset document
}

std::string MSCOFFLoader::getLongName(const std::string &stroffset)
{
    auto location = relpointer<const char>(m_longnameshdr, sizeof(ImageArchiveMemberHeader) + std::stoi(stroffset));

    if(!location)
        return nullptr;

    return location;
}

void MSCOFFLoader::readMemberHeaders()
{
    BufferView view = m_view.view(MSCOFF_SIGNATURE_SIZE);

    while(!view.eob())
    {
        const ImageArchiveMemberHeader* memberheader = static_cast<const ImageArchiveMemberHeader*>(view);
        std::string name = REDasm::rtrimmed(std::string(memberheader->name, 16));
        u32 size = REDasm::aligned(std::stoi(memberheader->size), 2);

        if(!size)
        {
            REDasm::log("Member " + REDasm::quoted(name) + " is empty");
            break;
        }

        this->readMember(memberheader, name);
        view += size + sizeof(ImageArchiveMemberHeader);
    }
}

const ImageSectionHeader* MSCOFFLoader::loadSegments(const ImageFileHeader *fileheader, const std::string &membername)
{
    const ImageSectionHeader* sectionheader = relpointer<ImageSectionHeader>(fileheader, sizeof(ImageFileHeader) + fileheader->SizeOfOptionalHeader);

    if(!sectionheader)
        return nullptr;

    bool ok = false;

    for(u32 i = 0; i < fileheader->NumberOfSections; i++)
    {
        const ImageSectionHeader& section = sectionheader[i];

        if(!section.PointerToRawData || !section.SizeOfRawData || !(section.Characteristics & IMAGE_SCN_CNT_CODE))
            continue;

        ok = true;
        u64 sectionoffset = fileoffset(fileheader) + section.PointerToRawData;
        std::string sectionname = membername + "_" + PEUtils::sectionName(reinterpret_cast<const char*>(&section.Name));
        m_document->segment(sectionname, sectionoffset, sectionoffset, section.SizeOfRawData, SegmentTypes::Code);
    }

    return ok ? sectionheader : nullptr;
}

void MSCOFFLoader::readMember(const ImageArchiveMemberHeader *memberhdr, const std::string& name)
{
    if(name == "/")
    {
        if(!m_firstlinkerhdr)
            m_firstlinkerhdr = memberhdr;
        else
            m_secondlinkerhdr = memberhdr;

        return;
    }
    if(name == "//")
    {
        m_longnameshdr = memberhdr;
        return;
    }

    std::string membername = (name.front() == '/') ? this->getLongName(name.substr(1)) : name;
    size_t idx = membername.find_last_of('\\');

    if(idx != std::string::npos)
        membername = membername.substr(idx + 1);

    idx = membername.find_last_of('.');

    if(idx != std::string::npos)
        membername = membername.substr(0, idx);

    const ImageFileHeader* fileheader = this->getMemberData<ImageFileHeader>(memberhdr);

    if(!fileheader->Machine || !fileheader->NumberOfSymbols)
        return;

    const u8* symboltable = relpointer<u8>(fileheader, fileheader->PointerToSymbolTable);

    if(!symboltable)
        return;

    const ImageSectionHeader* sectionheader = this->loadSegments(fileheader, membername);

    if(!sectionheader)
        return;

    m_machines.insert(fileheader->Machine);

    COFF::loadSymbols([&](const std::string& name, const COFF::COFF_Entry* entry) {
                      u16 idx = entry->e_scnum - 1;

                      if((idx < 0) || (idx >= fileheader->NumberOfSections))
                          return;

                      address_t address = fileoffset(fileheader) + sectionheader[idx].PointerToRawData + entry->e_value;
                      m_document->lock(address, name, SymbolTypes::Function);
    },
    symboltable,
    fileheader->NumberOfSymbols);

}

} // namespace REDasm
