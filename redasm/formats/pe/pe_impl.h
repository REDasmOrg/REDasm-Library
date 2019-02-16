#include "pe.h"
#include "pe_constants.h"
#include "pe_header.h"
#include "pe_analyzer.h"
#include "pe_debug.h"
#include "pe_utils.h"
#include "dotnet/dotnet.h"
#include "vb/vb_analyzer.h"
#include "borland/borland_version.h"
#include "../../support/coff/coff_symboltable.h"

namespace REDasm {

template<size_t b> FORMAT_PLUGIN_TEST(PeFormat<b>, ImageDosHeader)
{
    if((format->e_magic != IMAGE_DOS_SIGNATURE) || !view.inRange(format->e_lfanew))
        return false;

    const ImageNtHeaders* ntheaders = PeFormat::relpointer<const ImageNtHeaders>(format, format->e_lfanew);

    if(ntheaders->Signature != IMAGE_NT_SIGNATURE)
        return false;

    if(b == 32)
        return ntheaders->OptionalHeaderMagic == IMAGE_NT_OPTIONAL_HDR32_MAGIC;

    if(b == 64)
        return ntheaders->OptionalHeaderMagic == IMAGE_NT_OPTIONAL_HDR64_MAGIC;

    return false;
}

template<size_t b> PeFormat<b>::PeFormat(AbstractBuffer *buffer): FormatPluginT<ImageDosHeader>(buffer), m_dosheader(nullptr), m_ntheaders(nullptr), m_sectiontable(nullptr), m_datadirectory(nullptr)
{
    m_imagebase = m_sectionalignment = m_entrypoint = 0;
    m_petype = PeType::None;
}

template<size_t b> std::string PeFormat<b>::name() const { return "PE Format (" + std::to_string(b) + " bits)"; }
template<size_t b> u32 PeFormat<b>::bits() const { return b; }
template<size_t b> const DotNetReader *PeFormat<b>::dotNetReader() const { return m_dotnetreader.get(); }

template<size_t b> std::string PeFormat<b>::assembler() const
{
    if(m_petype == PeType::DotNet)
        return "cil";
    if(m_ntheaders->FileHeader.Machine == IMAGE_FILE_MACHINE_I386)
        return "x86_32";
    if(m_ntheaders->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64)
        return "x86_64";

    if(m_ntheaders->FileHeader.Machine == IMAGE_FILE_MACHINE_ARM)
    {
        if(m_ntheaders->OptionalHeaderMagic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
            return "arm64";

        return "arm";
    }

    return nullptr;
}

template<size_t b> Analyzer *PeFormat<b>::createAnalyzer(DisassemblerAPI *disassembler, const SignatureFiles &signatures) const
{
    if(m_petype == PeType::VisualBasic)
        return new VBAnalyzer(this->m_petype, disassembler, signatures);

    return new PEAnalyzer(this->m_petype, disassembler, signatures);
}

template<size_t b> void PeFormat<b>::load()
{
    m_dosheader = m_format;
    m_ntheaders = pointer<ImageNtHeaders>(m_dosheader->e_lfanew);
    m_sectiontable = IMAGE_FIRST_SECTION(m_ntheaders);

    if(b == 64)
        m_optionalheader = reinterpret_cast<ImageOptionalHeader*>(&m_ntheaders->OptionalHeader64);
    else
        m_optionalheader = reinterpret_cast<ImageOptionalHeader*>(&m_ntheaders->OptionalHeader32);

    m_imagebase = m_optionalheader->ImageBase;
    m_sectionalignment = m_optionalheader->SectionAlignment;
    m_entrypoint = m_imagebase + m_optionalheader->AddressOfEntryPoint;
    m_datadirectory = reinterpret_cast<ImageDataDirectory*>(&m_optionalheader->DataDirectory);

    this->loadSections();
    ImageCorHeader* corheader = this->checkDotNet();

    if(corheader && (corheader->MajorRuntimeVersion == 1))
    {
        REDasm::log(".NET 1.x is not supported");
        return;
    }
    else if(!corheader)
        this->loadDefault();
    else
        this->loadDotNet(reinterpret_cast<ImageCor20Header*>(corheader));
}

template<size_t b> u64 PeFormat<b>::rvaToOffset(u64 rva, bool *ok) const
{
    for(size_t i = 0; i < m_ntheaders->FileHeader.NumberOfSections; i++)
    {
        const ImageSectionHeader& section = m_sectiontable[i];

        if((rva >= section.VirtualAddress) && (rva < (section.VirtualAddress + section.Misc.VirtualSize)))
        {
            if(ok)
                *ok = (section.SizeOfRawData != 0);

            if(!section.SizeOfRawData) // Check if section not BSS
                break;

            return section.PointerToRawData + (rva - section.VirtualAddress);
        }
    }

    return rva;
}

template<size_t b> void PeFormat<b>::checkDelphi(const PEResources& peresources)
{
    PEResources::ResourceItem ri = peresources.find(PEResources::RCDATA);

    if(!ri.second)
        return;

    ri = peresources.find("PACKAGEINFO", ri);

    if(!ri.second)
        return;

    u64 datasize = 0;
    PackageInfoHeader* packageinfo = peresources.data<PackageInfoHeader>(ri, this->m_format,
                                                                         [this](address_t a, bool* ok) -> offset_t { return this->rvaToOffset(a, ok); },
                                                                         &datasize);

    if(!packageinfo)
    {
        REDasm::log("WARNING: Cannot parse 'PACKAGEINFO' header");
        return;
    }

    BorlandVersion borlandver(packageinfo, ri, datasize);

    if(borlandver.isDelphi())
        m_petype = PeType::Delphi;
    else if(borlandver.isTurboCpp())
        m_petype = PeType::TurboCpp;

    std::string sig = borlandver.getSignature();

    if(sig.empty())
        return;

    REDasm::log("Signature '" + sig + "' detected");
    m_signatures.push_back(sig);
}

template<size_t b> void PeFormat<b>::checkPeTypeHeuristic()
{
    if(this->m_petype != PeType::None)
        return;

    REDasm::log("Running heuristic PEType checks...");
    const Segment* segment = m_document->segmentByName(".data");

    if(!segment)
        return;

    BufferView view = this->viewSegment(segment);

    if(view.eob())
        return;

    auto res = view.find(".?AVCWinApp");

    if(!res.hasNext())
        return;

    res = view.find(".?AVCWnd");

    if(!res.hasNext())
        return;

    m_petype = PeType::Msvc;
}

template<size_t b> void PeFormat<b>::checkResources()
{
    const ImageDataDirectory& resourcedatadir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];

    if(!resourcedatadir.VirtualAddress)
        return;

    ImageResourceDirectory* resourcedir = RVA_POINTER(ImageResourceDirectory, resourcedatadir.VirtualAddress);
    PEResources peresources(resourcedir);
    this->checkDelphi(peresources);
}

template<size_t b> void PeFormat<b>::checkDebugInfo()
{
    const ImageDataDirectory& debuginfodir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];

    if(!debuginfodir.VirtualAddress)
        return;

    ImageDebugDirectory* debugdir = RVA_POINTER(ImageDebugDirectory, debuginfodir.VirtualAddress);
    u64 dbgoffset = 0;

    if(debugdir->AddressOfRawData)
    {
        bool ok = false;
        u64 offset = this->rvaToOffset(m_imagebase - debugdir->AddressOfRawData, &ok);

        if(ok)
            dbgoffset = offset;
    }

    if(!dbgoffset && debugdir->PointerToRawData)
        dbgoffset = debugdir->PointerToRawData;

    if(debugdir->Type == IMAGE_DEBUG_TYPE_UNKNOWN)
        REDasm::log("Debug info type: UNKNOWN");
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_COFF)
        REDasm::log("Debug info type: COFF");
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_CODEVIEW)
    {
        REDasm::log("Debug info type: CodeView");
        m_petype = PeType::Msvc;

        if(!m_view.inRange(dbgoffset))
            return;

        CVHeader* cvhdr = pointer<CVHeader>(dbgoffset);

        if(cvhdr->Signature == PE_PDB_NB10_SIGNATURE)
        {
            CvInfoPDB20* pdb20 = pointer<CvInfoPDB20>(dbgoffset);
            REDasm::log("PDB 2.0 @ " + std::string(reinterpret_cast<const char*>(&pdb20->PdbFileName)));
        }
        else if(cvhdr->Signature == PE_PDB_RSDS_SIGNATURE)
        {
            CvInfoPDB70* pdb70 = pointer<CvInfoPDB70>(dbgoffset);
            REDasm::log("PDB 7.0 @ " + std::string(reinterpret_cast<const char*>(&pdb70->PdbFileName)));
        }
        else
            REDasm::log("Unknown Signature: '" + std::string(reinterpret_cast<const char*>(&cvhdr->Signature), sizeof(u32)));
    }
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_FPO)
        REDasm::log("Debug info type: FPO");
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_MISC)
        REDasm::log("Debug info type: Misc");
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_EXCEPTION)
        REDasm::log("Debug info type: Exception");
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_FIXUP)
        REDasm::log("Debug info type: FixUp");
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_OMAP_TO_SRC)
        REDasm::log("Debug info type: OMAP to Src");
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_OMAP_FROM_SRC)
        REDasm::log("Debug info type: OMAP from Src");
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_BORLAND)
        REDasm::log("Debug info type: Borland");
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_RESERVED10)
        REDasm::log("Debug info type: Reserved10");
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_CLSID)
        REDasm::log("Debug info type: CLSID");
    else
        REDasm::log("Debug info type: " + REDasm::hex(debugdir->Type));
}

template<size_t b> ImageCorHeader* PeFormat<b>::checkDotNet()
{
    const ImageDataDirectory& dotnetdir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_DOTNET];

    if(!dotnetdir.VirtualAddress)
        return nullptr;

    ImageCorHeader* corheader = RVA_POINTER(ImageCorHeader, dotnetdir.VirtualAddress);

    if(corheader->cb < sizeof(ImageCorHeader))
        return nullptr;

    return corheader;
}

template<size_t b> void PeFormat<b>::loadDotNet(ImageCor20Header* corheader)
{
    m_petype = PeType::DotNet;

    if(!corheader->MetaData.VirtualAddress)
    {
        REDasm::log("Invalid .NET MetaData");
        return;
    }

    ImageCor20MetaData* cormetadata = RVA_POINTER(ImageCor20MetaData, corheader->MetaData.VirtualAddress);
    m_dotnetreader = std::make_unique<DotNetReader>(cormetadata);

    if(!m_dotnetreader->isValid())
        return;

    m_dotnetreader->iterateTypes([this](u32 rva, const std::string& name) {
        m_document->lockFunction(m_imagebase + rva, name);
    });
}

template<size_t b> void PeFormat<b>::loadDefault()
{
    this->loadExports();
    this->loadImports();
    this->loadTLS();
    this->loadConfig();
    this->loadSymbolTable();
    this->checkDebugInfo();
    this->checkResources();
    this->checkPeTypeHeuristic();

    m_document->entry(m_entrypoint);
}

template<size_t b> void PeFormat<b>::loadSections()
{
    for(size_t i = 0; i < m_ntheaders->FileHeader.NumberOfSections; i++)
    {
        const ImageSectionHeader& section = m_sectiontable[i];
        u32 flags = SegmentTypes::None;

        if((section.Characteristics & IMAGE_SCN_CNT_CODE) || (section.Characteristics & IMAGE_SCN_MEM_EXECUTE))
            flags |= SegmentTypes::Code;

        if((section.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) || (section.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA))
            flags |= SegmentTypes::Data;

        u64 size = section.SizeOfRawData;

        if(!section.SizeOfRawData)
        {
            flags |= SegmentTypes::Bss;
            size = section.Misc.VirtualSize;
        }

        u64 diff = size & m_sectionalignment;

        if(diff)
            size += m_sectionalignment - diff;

        std::string name = PEUtils::sectionName(reinterpret_cast<const char*>(section.Name));

        if(name.empty()) // Rename unnamed sections
            name = "sect" + std::to_string(i);

        m_document->segment(name, section.PointerToRawData, m_imagebase + section.VirtualAddress, size, flags);
    }

    Segment* segment = m_document->segment(m_entrypoint);

    if(segment) // Entry points always points to code segment
        segment->type |= SegmentTypes::Code;
}

template<size_t b> void PeFormat<b>::loadExports()
{
    const ImageDataDirectory& exportdir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

    if(!exportdir.VirtualAddress)
        return;

    bool ok = false;
    ImageExportDirectory* exporttable = RVA_POINTER_OK(ImageExportDirectory, exportdir.VirtualAddress, &ok);
    if(!ok)
        return;

    u32* functions = RVA_POINTER(u32, exporttable->AddressOfFunctions);
    u32* names = RVA_POINTER(u32, exporttable->AddressOfNames);
    u16* nameords = RVA_POINTER(u16, exporttable->AddressOfNameOrdinals);

    for(size_t i = 0; i < exporttable->NumberOfFunctions; i++)
    {
        if(!functions[i])
            continue;

        bool namedfunction = false;
        u64 funcep = m_imagebase + functions[i];
        const Segment* segment = m_document->segment(funcep);

        if(!segment)
            continue;

        u32 symboltype = segment->is(SegmentTypes::Code) ? SymbolTypes::ExportFunction :
                                                           SymbolTypes::ExportData;

        for(u64 j = 0; j < exporttable->NumberOfNames; j++)
        {
            if(nameords[j] != i)
                continue;

            namedfunction = true;
            m_document->lock(funcep, RVA_POINTER(const char, names[j]), symboltype);
            break;
        }

        if(namedfunction)
            continue;

        std::stringstream ss;
        ss << "Ordinal__" << std::uppercase << std::setw(4) << std::setfill('0') << std::setbase(16) << (exporttable->Base + i);
        m_document->lock(funcep, ss.str(), symboltype);
    }
}

template<size_t b> void PeFormat<b>::loadImports()
{
    const ImageDataDirectory& importdir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

    if(!importdir.VirtualAddress)
        return;

    ImageImportDescriptor* importtable = RVA_POINTER(ImageImportDescriptor, importdir.VirtualAddress);

    for(size_t i = 0; i < importtable[i].FirstThunk; i++)
        this->readDescriptor(importtable[i], b == 64 ? IMAGE_ORDINAL_FLAG64 : IMAGE_ORDINAL_FLAG32);
}

template<size_t b> void PeFormat<b>::loadConfig()
{
    const ImageDataDirectory& configdir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG];

    if(!configdir.VirtualAddress)
        return;

    ImageLoadConfigDirectory* loadconfigdir = RVA_POINTER(ImageLoadConfigDirectory, configdir.VirtualAddress);

    if(!loadconfigdir->SecurityCookie)
        return;

    m_document->lock(loadconfigdir->SecurityCookie, PE_SECURITY_COOKIE_SYMBOL, SymbolTypes::Data);
}

template<size_t b> void PeFormat<b>::loadTLS()
{
    const ImageDataDirectory& tlsdir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_TLS];

    if(!tlsdir.VirtualAddress)
        return;

    this->readTLSCallbacks(RVA_POINTER(ImageTlsDirectory, tlsdir.VirtualAddress));
}

template<size_t b> void PeFormat<b>::loadSymbolTable()
{
    if(!m_ntheaders->FileHeader.PointerToSymbolTable || !m_ntheaders->FileHeader.NumberOfSymbols)
        return;

    REDasm::log("Loading symbol table @ " + REDasm::hex(m_ntheaders->FileHeader.PointerToSymbolTable));

    COFF::loadSymbols([this](const std::string& name, COFF::COFF_Entry* entry) {
                      if(m_document->segmentByName(name)) // Ignore segment informations
                          return;

                      const Segment* segment = m_document->segmentAt(entry->e_scnum - 1);
                      address_t address = segment->address + entry->e_value;

                      if(segment->is(SegmentTypes::Code))// && (entry->e_sclass == C_EXT))
                      {
                          m_document->lock(address, name, SymbolTypes::Function);
                          return;
                      }

                      SymbolPtr symbol = m_document->symbol(address);

                      if(symbol && symbol->isImport())
                          return;

                      m_document->lock(address, name, symbol ? symbol->type : SymbolTypes::Data,
                                                     symbol ? symbol->tag : 0); // Copy symbol type & tag, if exists
    },

    pointer<u8>(m_ntheaders->FileHeader.PointerToSymbolTable),
    m_ntheaders->FileHeader.NumberOfSymbols);
}

template<size_t b> void PeFormat<b>::readTLSCallbacks(const ImageTlsDirectory *tlsdirectory)
{
    if(!tlsdirectory->AddressOfCallBacks)
        return;

    pe_integer_t* callbacks = addrpointer<pe_integer_t>(tlsdirectory->AddressOfCallBacks);

    for(pe_integer_t i = 0; *callbacks; i++, callbacks++)
        m_document->lock(*callbacks, "TlsCallback_" + std::to_string(i), SymbolTypes::Function);
}

template<size_t b> void PeFormat<b>::readDescriptor(const ImageImportDescriptor& importdescriptor, pe_integer_t ordinalflag)
{
    // Check if OFT exists
    ImageThunkData* thunk = RVA_POINTER(ImageThunkData, importdescriptor.OriginalFirstThunk ? importdescriptor.OriginalFirstThunk :
                                                                                              importdescriptor.FirstThunk);

    std::string descriptorname = RVA_POINTER(const char, importdescriptor.Name);
    std::transform(descriptorname.begin(), descriptorname.end(), descriptorname.begin(), ::tolower);

    if(descriptorname.find("msvbvm") != std::string::npos)
        this->m_petype = PeType::VisualBasic;
    else if(PEUtils::checkMsvcImport(descriptorname))
        this->m_petype = PeType::Msvc;

    for(size_t i = 0; thunk[i]; i++)
    {
        std::string importname;
        address_t address = m_imagebase + (importdescriptor.FirstThunk + (i * sizeof(ImageThunkData))); // Instructions refers to FT

        if(!(thunk[i] & ordinalflag))
        {
            bool ok = false;
            ImageImportByName* importbyname = RVA_POINTER_OK(ImageImportByName, thunk[i], &ok);

            if(!ok)
                continue;

            importname = PEUtils::importName(descriptorname, reinterpret_cast<const char*>(&importbyname->Name));
        }
        else
        {
            u16 ordinal = static_cast<u16>(ordinalflag ^ thunk[i]);

            if(!PEImports::importName<b>(descriptorname, ordinal, importname))
                importname = PEUtils::importName(descriptorname, ordinal);
            else
                importname = PEUtils::importName(descriptorname, importname);
        }

        m_document->lock(address, importname, SymbolTypes::Import);
    }
}

} // REDasm
