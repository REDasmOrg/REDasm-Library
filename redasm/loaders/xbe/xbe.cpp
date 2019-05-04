#include "xbe.h"
#include "../../support/ordinals.h"

#define XBE_XBOXKRNL_BASEADDRESS 0x80000000

namespace REDasm {

LOADER_PLUGIN_TEST(XbeLoader, XbeImageHeader)
{
    if((header->Magic != XBE_MAGIC_NUMBER) || !header->SectionHeader || !header->NumberOfSections)
        return false;

    return true;
}

XbeLoader::XbeLoader(AbstractBuffer *buffer): LoaderPluginT<XbeImageHeader>(buffer) { }
std::string XbeLoader::assembler() const { return "x86_32"; }

void XbeLoader::load()
{
    this->loadSections(this->memoryoffset<XbeSectionHeader>(m_header->SectionHeader));
    address_t entrypoint = 0;

    if(!this->decodeEP(m_header->EntryPoint, entrypoint))
    {
        REDasm::log("Cannot decode Entry Point");
        return;
    }

    if(!this->loadXBoxKrnl())
    {
        REDasm::log("Cannot load XBoxKrnl Imports");
        return;
    }

    m_document->entry(entrypoint);
    this->displayXbeInfo();
}

void XbeLoader::displayXbeInfo()
{
    auto* certificate = this->memoryoffset<XbeCertificate>(m_header->CertificateAddress);
    std::string title = REDasm::wtoa(&certificate->TitleName, XBE_TITLENAME_SIZE);

    if(!title.empty())
        REDasm::log("Game Title: " + REDasm::quoted(title));

    std::string s;

    if(certificate->GameRegion & XBE_GAME_REGION_RESTOFWORLD)
        s += "ALL";
    else
    {
        if(certificate->GameRegion & XBE_GAME_REGION_JAPAN)
            s += s.empty() ? "JAPAN" : ", JAPAN";

        if(certificate->GameRegion & XBE_GAME_REGION_NA)
            s += s.empty() ? "NORTH AMERICA" : ", NORTH AMERICA";
    }

    if(certificate->GameRegion & XBE_GAME_REGION_MANUFACTURING)
        s += s.empty() ? "DEBUG" : ", DEBUG";

    if(!s.empty())
        REDasm::log("Allowed Regions: " + s);
}

bool XbeLoader::decodeEP(u32 encodedep, address_t& ep)
{
    ep = encodedep ^ XBE_ENTRYPOINT_XOR_RETAIL;
    Segment* segment = m_document->segment(ep);

    if(!segment)
    {
        ep = encodedep ^ XBE_ENTRYPOINT_XOR_DEBUG;
        segment = m_document->segment(ep);

        if(segment)
            REDasm::log("Executable Type: DEBUG");
    }
    else
        REDasm::log("Executable Type: RETAIL");

    return segment != nullptr;
}

bool XbeLoader::decodeKernel(u32 encodedthunk, u32 &thunk)
{
    thunk = encodedthunk ^ XBE_KERNEL_XOR_RETAIL;
    Segment* segment = m_document->segment(thunk);

    if(!segment)
    {
        thunk = encodedthunk ^ XBE_KERNEL_XOR_DEBUG;
        segment = m_document->segment(thunk);
    }

    return segment != nullptr;
}

void XbeLoader::loadSections(XbeSectionHeader *sectionhdr)
{
    for(u32 i = 0; i < m_header->NumberOfSections; i++)
    {
        std::string sectname = this->memoryoffset<const char>(sectionhdr[i].SectionName);
        SegmentType secttype = SegmentType::None;

        if(sectionhdr[i].Flags.Executable)
        {
            if((sectname[0] == '.') && (sectname.find("data") != std::string::npos))
                secttype = SegmentType::Data;
            else
                secttype = SegmentType::Code;
        }
        else
            secttype = SegmentType::Data;

        if(!sectionhdr[i].RawSize)
            secttype = SegmentType::Bss;

        m_document->segment(sectname, sectionhdr[i].RawAddress, sectionhdr[i].VirtualAddress, sectionhdr[i].RawSize, secttype);
    }

    m_document->segment("XBOXKRNL", 0, XBE_XBOXKRNL_BASEADDRESS, 0x10000, SegmentType::Bss);
}

bool XbeLoader::loadXBoxKrnl()
{
    OrdinalsMap ordinals;
    REDasm::loadordinals(REDasm::makeLoaderPath("xbe", "xboxkrnl.json"), ordinals);
    u32 kernelimagethunk;

    if(!this->decodeKernel(m_header->KernelImageThunk, kernelimagethunk))
        return false;

    offset_location thunkoffset = this->offset(kernelimagethunk);

    if(!thunkoffset.valid)
        return false;

    u32* pthunk = this->pointer<u32>(thunkoffset);

    while(*pthunk)
    {
        std::string ordinalname = REDasm::ordinal(ordinals, *pthunk ^ XBE_ORDINAL_FLAG, "XBoxKrnl!");
        m_document->lock(*pthunk, ordinalname, SymbolTypes::Import);
        pthunk++;
    }

    return true;
}

} // namespace REDasm
