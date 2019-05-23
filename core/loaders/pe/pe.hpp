#pragma once

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

template<size_t b> LOADER_PLUGIN_TEST(PELoader<b>, ImageDosHeader)
{
    if((header->e_magic != IMAGE_DOS_SIGNATURE) || !request.view.inRange(header->e_lfanew))
        return false;

    const ImageNtHeaders* ntheaders = PELoader::relpointer<const ImageNtHeaders>(header, header->e_lfanew);

    if(ntheaders->Signature != IMAGE_NT_SIGNATURE)
        return false;

    if(b == 32)
        return ntheaders->OptionalHeaderMagic == IMAGE_NT_OPTIONAL_HDR32_MAGIC;

    if(b == 64)
        return ntheaders->OptionalHeaderMagic == IMAGE_NT_OPTIONAL_HDR64_MAGIC;

    return false;
}

template<size_t b> PELoader<b>::PELoader(AbstractBuffer *buffer): LoaderPluginT<ImageDosHeader>(buffer), m_dosheader(nullptr), m_ntheaders(nullptr), m_sectiontable(nullptr), m_datadirectory(nullptr)
{
    m_imagebase = m_sectionalignment = m_entrypoint = 0;
    m_classifier.setBits(b);

    m_validimportsections.insert(".text");
    m_validimportsections.insert(".idata");
    m_validimportsections.insert(".rdata");
}

template<size_t b> const DotNetReader *PELoader<b>::dotNetReader() const { return m_dotnetreader.get(); }
template<size_t b> address_t PELoader<b>::rvaToVa(address_t rva) const { return rva + m_imagebase; }
template<size_t b> address_t PELoader<b>::vaToRva(address_t va) const { return va - m_imagebase; }

template<size_t b> std::string PELoader<b>::assembler() const
{
    if(m_classifier.checkDotNet())
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

template<size_t b> Analyzer *PELoader<b>::createAnalyzer(DisassemblerAPI *disassembler) const
{
    if(m_classifier.checkVisualBasic())
        return new VBAnalyzer(&m_classifier, disassembler);

    return new PEAnalyzer(&m_classifier, disassembler);
}

template<size_t b> void PELoader<b>::load()
{
    m_dosheader = m_header;
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

    if(m_classifier.checkDotNet() == PEClassifications::DotNet_1)
        REDasm::log(".NET 1.x is not supported");
    else if(!corheader)
        this->loadDefault();
    else
        this->loadDotNet(reinterpret_cast<ImageCor20Header*>(corheader));

    m_classifier.display();
}

template<size_t b> void PELoader<b>::checkResources()
{
    const ImageDataDirectory& resourcedatadir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];

    if(!resourcedatadir.VirtualAddress)
        return;

    ImageResourceDirectory* resourcedir = this->rvaPointer<ImageResourceDirectory>(resourcedatadir.VirtualAddress);

    if(!resourcedir)
        return;

    PEResources peresources(resourcedir);
    m_classifier.classifyDelphi(m_dosheader, m_ntheaders, resourcedir);
}

template<size_t b> void PELoader<b>::checkDebugInfo()
{
    const ImageDataDirectory& debuginfodir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];

    if(!debuginfodir.VirtualAddress)
        return;

    ImageDebugDirectory* debugdir = this->rvaPointer<ImageDebugDirectory>(debuginfodir.VirtualAddress);

    if(!debugdir)
        return;

    u64 dbgoffset = 0;

    if(debugdir->AddressOfRawData)
    {
        offset_location offset = PEUtils::rvaToOffset(m_ntheaders, m_imagebase - debugdir->AddressOfRawData);

        if(offset.valid)
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
        m_classifier.classifyVisualStudio();

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
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_VC_FEATURE)
        REDasm::log("Debug info type: VC Feature");
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_POGO)
        REDasm::log("Debug info type: POGO");
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_ILTCG)
        REDasm::log("Debug info type: ILTCG");
    else if(debugdir->Type == IMAGE_DEBUG_TYPE_REPRO)
        REDasm::log("Debug info type: REPRO");
    else
        REDasm::log("Unknown Debug info type (value " + REDasm::hex(debugdir->Type, 32, true) + ")");
}

template<size_t b> ImageCorHeader* PELoader<b>::checkDotNet()
{
    const ImageDataDirectory& dotnetdir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_DOTNET];

    if(!dotnetdir.VirtualAddress)
        return nullptr;

    ImageCorHeader* corheader = this->rvaPointer<ImageCorHeader>(dotnetdir.VirtualAddress);
    m_classifier.classifyDotNet(corheader);
    return corheader;
}

template<size_t b> void PELoader<b>::loadDotNet(ImageCor20Header* corheader)
{
    if(!corheader->MetaData.VirtualAddress)
    {
        REDasm::log("Invalid .NET MetaData");
        return;
    }

    ImageCor20MetaData* cormetadata = this->rvaPointer<ImageCor20MetaData>(corheader->MetaData.VirtualAddress);

    if(!cormetadata)
        return;

    m_dotnetreader = std::make_unique<DotNetReader>(cormetadata);

    if(!m_dotnetreader->isValid())
        return;

    m_dotnetreader->iterateTypes([this](u32 rva, const std::string& name) {
        m_document->lockFunction(m_imagebase + rva, name);
    });
}

template<size_t b> void PELoader<b>::loadDefault()
{
    this->loadExports();

    if(!this->loadImports())
        REDasm::log("WARNING: This file seems to be PACKED");

    this->loadTLS();
    this->loadConfig();
    this->loadExceptions();
    this->loadSymbolTable();
    this->checkDebugInfo();
    this->checkResources();

    m_document->entry(m_entrypoint);
    m_classifier.classify(m_ntheaders);
    m_signatures = m_classifier.signatures();
}

template<size_t b> void PELoader<b>::loadSections()
{
    for(size_t i = 0; i < m_ntheaders->FileHeader.NumberOfSections; i++)
    {
        const ImageSectionHeader& section = m_sectiontable[i];
        SegmentType flags = SegmentType::None;

        if((section.Characteristics & IMAGE_SCN_CNT_CODE) || (section.Characteristics & IMAGE_SCN_MEM_EXECUTE))
            flags |= SegmentType::Code;

        if((section.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) || (section.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA))
            flags |= SegmentType::Data;

        u64 vsize = section.Misc.VirtualSize;

        if(!section.SizeOfRawData)
            flags |= SegmentType::Bss;

        u64 diff = vsize % m_sectionalignment;

        if(diff)
            vsize += m_sectionalignment - diff;

        std::string name = PEUtils::sectionName(reinterpret_cast<const char*>(section.Name));

        if(name.empty()) // Rename unnamed sections
            name = "sect" + std::to_string(i);

        m_document->segment(name, section.PointerToRawData, m_imagebase + section.VirtualAddress, section.SizeOfRawData, vsize, flags);
    }

    Segment* segment = m_document->segment(m_entrypoint);

    if(segment) // Entry points always points to code segment
        segment->type |= SegmentType::Code;
}

template<size_t b> void PELoader<b>::loadExports()
{
    const ImageDataDirectory& exportdir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

    if(!exportdir.VirtualAddress)
        return;

    ImageExportDirectory* exporttable = this->rvaPointer<ImageExportDirectory>(exportdir.VirtualAddress);

    if(!exporttable)
        return;

    u32* functions = this->rvaPointer<u32>(exporttable->AddressOfFunctions);
    u32* names = this->rvaPointer<u32>(exporttable->AddressOfNames);
    u16* nameords = this->rvaPointer<u16>(exporttable->AddressOfNameOrdinals);

    if(!functions || !names || !nameords)
    {
        REDasm::log("Corrupted export table");
        return;
    }

    for(size_t i = 0; i < exporttable->NumberOfFunctions; i++)
    {
        if(!functions[i])
            continue;

        bool namedfunction = false;
        u64 funcep = m_imagebase + functions[i];
        const Segment* segment = m_document->segment(funcep);

        if(!segment)
            continue;

        SymbolType symboltype = segment->is(SegmentType::Code) ? SymbolType::ExportFunction :
                                                           SymbolType::ExportData;

        for(pe_integer_t j = 0; j < exporttable->NumberOfNames; j++)
        {
            if(nameords[j] != i)
                continue;

            namedfunction = true;
            m_document->lock(funcep, this->rvaPointer<const char>(names[j]), symboltype);
            break;
        }

        if(namedfunction)
            continue;

        std::stringstream ss;
        ss << "Ordinal__" << std::uppercase << std::setw(4) << std::setfill('0') << std::setbase(16) << (exporttable->Base + i);
        m_document->lock(funcep, ss.str(), symboltype);
    }
}

template<size_t b> bool PELoader<b>::loadImports()
{
    const ImageDataDirectory& importdir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

    if(!importdir.VirtualAddress)
        return false;

    ImageImportDescriptor* importtable = this->rvaPointer<ImageImportDescriptor>(importdir.VirtualAddress);

    if(!importtable)
        return false;

    for(size_t i = 0; i < importtable[i].FirstThunk; i++)
        this->readDescriptor(importtable[i], b == 64 ? IMAGE_ORDINAL_FLAG64 : IMAGE_ORDINAL_FLAG32);

    Segment* segment = m_document->segment(m_imagebase + importdir.VirtualAddress);
    return segment && (m_validimportsections.find(segment->name) != m_validimportsections.end());
}

template<size_t b> void PELoader<b>::loadExceptions()
{
    const ImageDataDirectory& exceptiondir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];

    if(!exceptiondir.VirtualAddress || !exceptiondir.Size)
        return;

    ImageRuntimeFunctionEntry* runtimeentry = this->rvaPointer<ImageRuntimeFunctionEntry>(exceptiondir.VirtualAddress);

    if(!runtimeentry)
        return;

    u64 c = 0, csize = 0;

    for(pe_integer_t i = 0; csize < exceptiondir.Size; i++, csize += sizeof(ImageRuntimeFunctionEntry))
    {
        address_t va = m_imagebase + runtimeentry[i].BeginAddress;

        if(!m_document->segment(va) || (runtimeentry[i].UnwindInfoAddress & 1))
            continue;

        UnwindInfo* unwindinfo = this->rvaPointer<UnwindInfo>(runtimeentry[i].UnwindInfoAddress & ~1u);

        if(!unwindinfo || (unwindinfo->Flags & UNW_FLAG_CHAININFO))
            continue;

        m_document->function(va);
        c++;
    }

    if(c)
        REDasm::log("Found " + std::to_string(c) + " function(s) in Exception Directory");
}

template<size_t b> void PELoader<b>::loadConfig()
{
    const ImageDataDirectory& configdir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG];

    if(!configdir.VirtualAddress)
        return;

    ImageLoadConfigDirectory* loadconfigdir = this->rvaPointer<ImageLoadConfigDirectory>(configdir.VirtualAddress);

    if(!loadconfigdir || !loadconfigdir->SecurityCookie)
        return;

    m_document->lock(loadconfigdir->SecurityCookie, PE_SECURITY_COOKIE_SYMBOL, SymbolType::Data);
}

template<size_t b> void PELoader<b>::loadTLS()
{
    const ImageDataDirectory& tlsdir = m_datadirectory[IMAGE_DIRECTORY_ENTRY_TLS];

    if(!tlsdir.VirtualAddress)
        return;

    ImageTlsDirectory* imagetlsdir = this->rvaPointer<ImageTlsDirectory>(tlsdir.VirtualAddress);

    if(imagetlsdir)
        this->readTLSCallbacks(imagetlsdir);
}

template<size_t b> void PELoader<b>::loadSymbolTable()
{
    if(!m_ntheaders->FileHeader.PointerToSymbolTable || !m_ntheaders->FileHeader.NumberOfSymbols)
        return;

    REDasm::log("Loading symbol table @ " + REDasm::hex(m_ntheaders->FileHeader.PointerToSymbolTable));

    COFF::loadSymbols([&](const std::string& name, const COFF::COFF_Entry* entry) {
                      const Segment& segment = m_document->segments()[entry->e_scnum - 1];
                      m_document->lock(segment.address + entry->e_value, name, SymbolType::Function);
    },
    pointer<u8>(m_ntheaders->FileHeader.PointerToSymbolTable),
    m_ntheaders->FileHeader.NumberOfSymbols);
}

template<size_t b> void PELoader<b>::readTLSCallbacks(const ImageTlsDirectory *tlsdirectory)
{
    if(!tlsdirectory->AddressOfCallBacks)
        return;

    pe_integer_t* callbacks = addrpointer<pe_integer_t>(tlsdirectory->AddressOfCallBacks);

    for(pe_integer_t i = 0; *callbacks; i++, callbacks++)
        m_document->lock(*callbacks, "TlsCallback_" + std::to_string(i), SymbolType::Function);
}

template<size_t b> void PELoader<b>::readDescriptor(const ImageImportDescriptor& importdescriptor, pe_integer_t ordinalflag)
{
    // Check if OFT exists
    ImageThunkData* thunk = this->rvaPointer<ImageThunkData>(importdescriptor.OriginalFirstThunk ? importdescriptor.OriginalFirstThunk :
                                                                                                   importdescriptor.FirstThunk);

    if(!thunk)
        return;

    std::string descriptorname = this->rvaPointer<const char>(importdescriptor.Name);
    std::transform(descriptorname.begin(), descriptorname.end(), descriptorname.begin(), ::tolower);
    m_classifier.classifyImport(descriptorname);

    for(size_t i = 0; thunk[i]; i++)
    {
        std::string importname;
        address_t address = m_imagebase + (importdescriptor.FirstThunk + (i * sizeof(ImageThunkData))); // Instructions refers to FT

        if(!(thunk[i] & ordinalflag))
        {
            ImageImportByName* importbyname = this->rvaPointer<ImageImportByName>(thunk[i]);

            if(!importbyname)
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

        m_document->lock(address, importname, SymbolType::Import);
    }
}

} // REDasm
