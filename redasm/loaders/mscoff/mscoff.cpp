#include "mscoff.h"

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
const MSCOFFLoader::FunctionList &MSCOFFLoader::functions() const { return m_functions; }

const RelocationList *MSCOFFLoader::relocations(const ImageSectionHeader *section) const
{
    auto it = m_relocations.find(section);

    if(it != m_relocations.end())
        return &it->second;

    return nullptr;
}

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

void MSCOFFLoader::readRelocations(const ImageSectionHeader *sectionheader, const ImageFileHeader* fileheader)
{
    const ImageRelocation* relocations = nullptr;

    if(!sectionheader->PointerToRelocations || !sectionheader->NumberOfRelocations)
        return;

    auto it = m_relocations.find(sectionheader);

    if(it == m_relocations.end())
        m_relocations[sectionheader] = { };

    relocations = relpointer<ImageRelocation>(fileheader, sectionheader->PointerToRelocations);

    if(fileheader->Machine == IMAGE_FILE_MACHINE_I386)
        this->readRelocations_x86_32(sectionheader, relocations);
    else if(fileheader->Machine == IMAGE_FILE_MACHINE_IA64)
        this->readRelocations_x86_64(sectionheader, relocations);
    else if(fileheader->Machine == IMAGE_FILE_MACHINE_ARM)
        this->readRelocations_arm(sectionheader, relocations);
    else
        REDasm::log("Unsupported relocations for machine type: " + REDasm::hex(fileheader->Machine));
}

void MSCOFFLoader::readRelocations_x86_32(const ImageSectionHeader *sectionheader, const ImageRelocation *relocations)
{
    for(u32 i = 0; i < sectionheader->NumberOfRelocations; i++)
    {
        switch(relocations[i].Type)
        {
            case IMAGE_REL_I386_DIR16:
            case IMAGE_REL_I386_REL16:
            case IMAGE_REL_I386_REL32:
            case IMAGE_REL_I386_DIR32:
            case IMAGE_REL_I386_DIR32NB:
            case IMAGE_REL_I386_SECREL:
            case IMAGE_REL_BASED_HIGHLOW:
            case IMAGE_REL_BASED_HIGHADJ:
                m_relocations[sectionheader].push_back({ relocations[i].VirtualAddress, sizeof(u32) });
                break;

            case IMAGE_REL_I386_SECTION:
                m_relocations[sectionheader].push_back({ relocations[i].VirtualAddress, sizeof(u16) });
                break;

            default:
                break;
        }
    }
}

void MSCOFFLoader::readRelocations_x86_64(const ImageSectionHeader *sectionheader, const ImageRelocation *relocations)
{
    for(u32 i = 0; i < sectionheader->NumberOfRelocations; i++)
    {
        switch(relocations[i].Type)
        {
            case IMAGE_REL_AMD64_ADDR32:
            case IMAGE_REL_AMD64_ADDR32NB:
            case IMAGE_REL_AMD64_REL32:
            case IMAGE_REL_AMD64_REL32_1:
            case IMAGE_REL_AMD64_REL32_2:
            case IMAGE_REL_AMD64_REL32_3:
            case IMAGE_REL_AMD64_REL32_4:
            case IMAGE_REL_AMD64_REL32_5:
            case IMAGE_REL_AMD64_SECREL:
            case IMAGE_REL_AMD64_SREL32:
            case IMAGE_REL_AMD64_SSPAN32:
                m_relocations[sectionheader].push_back({ relocations[i].VirtualAddress, sizeof(u32) });
                break;

            case IMAGE_REL_AMD64_SECREL7:
                m_relocations[sectionheader].push_back({ relocations[i].VirtualAddress, sizeof(u16) });
                break;

            case IMAGE_REL_AMD64_SECTION:
                m_relocations[sectionheader].push_back({ relocations[i].VirtualAddress, sizeof(u16) });
                break;

            case IMAGE_REL_AMD64_ADDR64:
                m_relocations[sectionheader].push_back({ relocations[i].VirtualAddress, sizeof(u64) });
                break;

            default:
                break;
        }
    }
}

void MSCOFFLoader::readRelocations_arm(const ImageSectionHeader *sectionheader, const ImageRelocation *relocations)
{
    for(u32 i = 0; i < sectionheader->NumberOfRelocations; i++)
    {
        switch(relocations[i].Type)
        {
            case IMAGE_REL_ARM_ADDR32:
            case IMAGE_REL_ARM_ADDR32NB:
            case IMAGE_REL_ARM_BRANCH24:
            case IMAGE_REL_ARM_SECREL:
            case IMAGE_REL_ARM_MOV32:
            case IMAGE_REL_THUMB_MOV32:
            case IMAGE_REL_THUMB_BRANCH20:
            case IMAGE_REL_THUMB_BRANCH24:
            case IMAGE_REL_THUMB_BLX23:
                m_relocations[sectionheader].push_back({ relocations[i].VirtualAddress, sizeof(u32) });
                break;

            case IMAGE_REL_ARM_BRANCH11:
            case IMAGE_REL_ARM_SECTION:
                m_relocations[sectionheader].push_back({ relocations[i].VirtualAddress, sizeof(u16) });
                break;

            default:
                break;
        }
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

    for(u32 i = 0; i < fileheader->NumberOfSections; i++)
        this->readRelocations(&sectionheader[i], fileheader);

    COFF::loadSymbols([&](const std::string& name, const COFF::COFF_Entry* entry) {
                      u16 idx = static_cast<u16>(entry->e_scnum - 1);

                      if(idx >= fileheader->NumberOfSections)
                          return;

                      offset_t offset = fileoffset(fileheader) + sectionheader[idx].PointerToRawData + entry->e_value;
                      m_document->lock(static_cast<address_t>(offset), name, SymbolTypes::Function);
                      m_functions.push_front({ name, offset, sectionheader[idx].SizeOfRawData - entry->e_value, &sectionheader[idx] });
    },
    symboltable, fileheader->NumberOfSymbols);
}

} // namespace REDasm
