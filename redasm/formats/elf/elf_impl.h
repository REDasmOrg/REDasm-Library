#include "elf.h"

namespace REDasm {

template<ELF_PARAMS_T> FORMAT_PLUGIN_TEST(ElfFormat<ELF_PARAMS_D>, EHDR)
{
    if((format->e_ident[EI_MAG0] != ELFMAG0) || (format->e_ident[EI_MAG1] != ELFMAG1))
        return false;

    if((format->e_ident[EI_MAG2] != ELFMAG2) || (format->e_ident[EI_MAG3] != ELFMAG3))
        return false;

    if(format->e_ident[EI_VERSION] != EV_CURRENT)
        return false;

    return (format->e_ident[EI_CLASS] == ELFCLASS32) || (format->e_ident[EI_CLASS] == ELFCLASS64);
}

template<ELF_PARAMS_T> ElfFormat<ELF_PARAMS_D>::ElfFormat(Buffer& buffer): FormatPluginT<EHDR>(buffer), m_shdr(NULL)
{
    m_skipsections.insert(".comment");
    m_skipsections.insert(".attribute");
}

template<ELF_PARAMS_T> u32 ElfFormat<ELF_PARAMS_D>::bits() const
{
    if(this->m_format->e_ident[EI_CLASS] == ELFCLASS32)
        return 32;

    if(this->m_format->e_ident[EI_CLASS] == ELFCLASS64)
        return 64;

    return 0;
}

template<ELF_PARAMS_T> const char* ElfFormat<ELF_PARAMS_D>::assembler() const
{
    switch(this->m_format->e_machine)
    {
        case EM_386:
            return "x86_32";

        case EM_X86_64:
            return "x86_64";

        case EM_MIPS:
        {
            if(this->m_format->e_flags & EF_MIPS_ABI_EABI64)
                return "mips64le";

            return "mips32le";
        }

        case EM_ARM:
            return this->bits() == 32 ? "metaarm" : "arm64";

        default:
            break;
    }

    return NULL;
}

template<ELF_PARAMS_T> void ElfFormat<ELF_PARAMS_D>::load()
{
    this->m_shdr = POINTER(SHDR, this->m_format->e_shoff);
    this->m_phdr = POINTER(PHDR, this->m_format->e_phoff);
    this->loadSegments();
    this->parseSegments();
    this->checkProgramHeader();
    this->m_document->entry(this->m_format->e_entry);
}

template<ELF_PARAMS_T> Analyzer* ElfFormat<ELF_PARAMS_D>::createAnalyzer(DisassemblerAPI *disassembler, const SignatureFiles &signatures) const
{
    return new ElfAnalyzer(disassembler, signatures);
}

template<ELF_PARAMS_T> bool ElfFormat<ELF_PARAMS_D>::relocate(u64 symidx, u64* value) const
{
    for(u64 i = 0; i < this->m_format->e_shnum; i++)
    {
        const SHDR& shdr = this->m_shdr[i];

        if((shdr.sh_type != SHT_REL) && (shdr.sh_type != SHT_RELA))
            continue;

        offset_t offset = shdr.sh_offset, endoffset = offset + shdr.sh_size;

        while(offset < endoffset)
        {
            REL* rel = POINTER(REL, offset);
            u64 sym = this->relocationSymbol(rel);

            if(sym == symidx)
            {
                *value = rel->r_offset;
                return true;
            }

            offset += (shdr.sh_type == SHT_REL) ? sizeof(REL) : sizeof(RELA);
        }
    }

    return false;
}

template<ELF_PARAMS_T> void ElfFormat<ELF_PARAMS_D>::loadSegments()
{
    const SHDR& shstr = ELF_STRING_TABLE;

    for(u64 i = 0; i < this->m_format->e_shnum; i++)
    {
        const SHDR& shdr = this->m_shdr[i];

        if((shdr.sh_type == SHT_NULL) || (shdr.sh_type == SHT_STRTAB) || (shdr.sh_type == SHT_SYMTAB))
            continue;

        u32 type = SegmentTypes::Data;

        if((shdr.sh_type & SHT_PROGBITS) && (shdr.sh_flags & SHF_EXECINSTR))
            type = SegmentTypes::Code;

        if(shdr.sh_type & SHT_NOBITS)
            type = SegmentTypes::Bss;

        std::string name = ELF_STRING(&shstr, shdr.sh_name);
        bool skip = false;

        for(const std::string& s : m_skipsections)
        {
            if(name.find(s) == std::string::npos)
                continue;

            skip = true;
            break;
        }

        if(!skip)
            this->m_document->segment(name, shdr.sh_offset, shdr.sh_addr, shdr.sh_size, type);
    }
}

template<ELF_PARAMS_T> void ElfFormat<ELF_PARAMS_D>::checkProgramHeader()
{
    if(this->m_format->e_shnum)
        return;

    for(u64 i = 0; i < this->m_format->e_phnum; i++)
    {
        const PHDR& phdr = this->m_phdr[i];

        if((phdr.p_type != PT_LOAD) || !phdr.p_memsz)
            continue;

        this->m_document->segment("LOAD", phdr.p_offset, phdr.p_vaddr, phdr.p_memsz, SegmentTypes::Code);
    }
}

template<ELF_PARAMS_T> void ElfFormat<ELF_PARAMS_D>::loadSymbols(const SHDR& shdr)
{
    offset_t offset = shdr.sh_offset, endoffset = offset + shdr.sh_size;
    const SHDR& shstr = shdr.sh_link ? this->m_shdr[shdr.sh_link] : ELF_STRING_TABLE;

    for(u64 idx = 0; offset < endoffset; idx++)
    {
        bool isrelocated = false;
        SYM* sym = POINTER(SYM, offset);
        u8 info = ELF_ST_TYPE(sym->st_info);
        u64 symvalue = sym->st_value;

        if(!sym->st_name)
        {
            offset += sizeof(SYM);
            continue;
        }

        if(!symvalue)
            isrelocated = this->relocate(idx, &symvalue);

        std::string symname = ELF_STRING(&shstr, sym->st_name);

        if(!isrelocated)
        {
            bool isexport = false;
            u8 bind = ELF_ST_BIND(sym->st_info);
            u8 visibility = ELF_ST_VISIBILITY(sym->st_other);

            if(visibility == STV_DEFAULT)
                isexport = (bind == STB_GLOBAL) || (bind == STB_WEAK);
            else if(bind == STB_GLOBAL)
                isexport = true;

            if(isexport)
                this->m_document->lock(symvalue, symname, (info == STT_FUNC) ? SymbolTypes::ExportFunction : SymbolTypes::ExportData);
            else if(info == STT_FUNC)
                this->m_document->lock(symvalue, symname);
            else if(info == STT_OBJECT)
            {
                const Segment* segment = this->m_document->segment(symvalue);

                if(segment && !segment->is(SegmentTypes::Code))
                    this->m_document->lock(symvalue, symname, SymbolTypes::Data);
            }
        }
        else
            this->m_document->lock(symvalue, symname, SymbolTypes::Import);

        offset += sizeof(SYM);
    }
}

template<ELF_PARAMS_T> void ElfFormat<ELF_PARAMS_D>::parseSegments()
{
    for(u64 i = 0; i < this->m_format->e_shnum; i++)
    {
        const SHDR& shdr = this->m_shdr[i];

        if(shdr.sh_offset && ((shdr.sh_type == SHT_SYMTAB) || (shdr.sh_type == SHT_DYNSYM)))
        {
            const SHDR& shstr = ELF_STRING_TABLE;
            REDasm::log("Section" + REDasm::quoted(ELF_STRING(&shstr, shdr.sh_name)) + " contains a "
                        "symbol table @ offset " + REDasm::hex(shdr.sh_offset, this->bits()));

            this->loadSymbols(shdr);
        }
    }
}

} // namespace REDasm
