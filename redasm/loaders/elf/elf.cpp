#pragma once
#include "elf.h"
#include "elf_header.h"

namespace REDasm {

template<size_t b, endianness_t e> LOADER_PLUGIN_TEST(ELF_ARG(ELFLoader<b, e>), ELF_ARG(Elf_Ehdr<b, e>))
{
    if((header->e_ident[EI_MAG0] != ELFMAG0) || (header->e_ident[EI_MAG1] != ELFMAG1))
        return false;

    if((header->e_ident[EI_MAG2] != ELFMAG2) || (header->e_ident[EI_MAG3] != ELFMAG3))
        return false;

    if(header->e_ident[EI_VERSION] != EV_CURRENT)
        return false;

    u8 elfendianness = (e == Endianness::BigEndian) ? ELFDATA2MSB : // MSB -> BigEndian
                                                      ELFDATA2LSB;  // LSB -> LittleEndian

    if(elfendianness != header->e_ident[EI_DATA])
        return false;

    if(b == 32)
        return (header->e_ident[EI_CLASS] == ELFCLASS32);
    else if(b == 64)
        return (header->e_ident[EI_CLASS] == ELFCLASS64);

    return false;
}

template<size_t b, endianness_t e> ELFLoader<b, e>::ELFLoader(AbstractBuffer *buffer): LoaderPluginT<EHDR>(buffer), m_shdr(nullptr)
{
    m_skipsections.insert(".pdr");
    m_skipsections.insert(".comment");
    m_skipsections.insert(".attribute");
}

template<size_t b, endianness_t e> std::string ELFLoader<b, e>::assembler() const
{
    switch(this->m_header->e_machine)
    {
        case EM_AVR:
            return "avr8";

        case EM_386:
            return "x86_32";

        case EM_X86_64:
            return "x86_64";

        case EM_MIPS:
        {
            if(this->m_header->e_flags & EF_MIPS_ABI_EABI64)
                return e == Endianness::BigEndian ? "mips64be" : "mips64le";

            return e == Endianness::BigEndian ? "mips32be" : "mips32le";
        }

        case EM_ARM:
        {
            if(this->m_header->e_ident[EI_CLASS] == ELFCLASS64)
                return "arm64";

            return "metaarm";
        }

        default:
            break;
    }

    return nullptr;
}

template<size_t b, endianness_t e> void ELFLoader<b, e>::load()
{
    this->m_shdr = ELF_POINTER(SHDR, this->m_header->e_shoff);
    this->m_phdr = ELF_POINTER(PHDR, this->m_header->e_phoff);
    this->loadSegments();
    this->parseSegments();
    this->checkProgramHeader();
    this->checkArray();
    this->m_document->entry(this->m_header->e_entry);
}

template<size_t b, endianness_t e> Analyzer* ELFLoader<b, e>::createAnalyzer(DisassemblerAPI *disassembler) const { return new ElfAnalyzer(disassembler); }

template<size_t b, endianness_t e> u64 ELFLoader<b, e>::relocationSymbol(const REL* rel) const
{
    if(b == 64)
        return ELF64_R_SYM(rel->r_info);

    return ELF32_R_SYM(rel->r_info);
}

template<size_t b, endianness_t e> bool ELFLoader<b, e>::relocate(u64 symidx, u64* value) const
{
    for(u64 i = 0; i < this->m_header->e_shnum; i++)
    {
        const SHDR& shdr = this->m_shdr[i];

        if((shdr.sh_type != SHT_REL) && (shdr.sh_type != SHT_RELA))
            continue;

        offset_t offset = shdr.sh_offset, endoffset = offset + shdr.sh_size;

        while(offset < endoffset)
        {
            REL* rel = ELF_POINTER(REL, offset);
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

template<size_t b, endianness_t e> void ELFLoader<b, e>::loadSegments()
{
    const SHDR& shstr = ELF_STRING_TABLE;

    for(u64 i = 0; i < this->m_header->e_shnum; i++)
    {
        const SHDR& shdr = this->m_shdr[i];

        if((shdr.sh_type == SHT_NULL) || (shdr.sh_type == SHT_STRTAB) || (shdr.sh_type == SHT_SYMTAB))
            continue;

        u32 type = SegmentTypes::Data;

        if((shdr.sh_type == SHT_PROGBITS) && (shdr.sh_flags & SHF_EXECINSTR))
            type = SegmentTypes::Code;

        if(shdr.sh_type == SHT_NOBITS)
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

template<size_t b, endianness_t e> void ELFLoader<b, e>::checkProgramHeader()
{
    if(this->m_header->e_shnum)
        return;

    for(u64 i = 0; i < this->m_header->e_phnum; i++)
    {
        const PHDR& phdr = this->m_phdr[i];

        if((phdr.p_type != PT_LOAD) || !phdr.p_memsz)
            continue;

        this->m_document->segment("LOAD", phdr.p_offset, phdr.p_vaddr, phdr.p_memsz, SegmentTypes::Code);
    }
}

template<size_t b, endianness_t e> void ELFLoader<b, e>::checkArray()
{
    for(u64 i = 0; i < this->m_header->e_shnum; i++)
    {
        const SHDR& shdr = this->m_shdr[i];
        std::string prefix;

        if(shdr.sh_type == SHT_INIT_ARRAY)
            prefix = "_init_";
        else if(shdr.sh_type == SHT_FINI_ARRAY)
            prefix = "_fini_";
        else if(shdr.sh_type == SHT_FINI_ARRAY)
            prefix = "_preinit_";
        else
            continue;

        ADDR* arr = ELF_POINTER(ADDR, shdr.sh_offset);

        if(!arr)
            continue;

        for(ADDR j = 0; j < shdr.sh_size; j += b, arr++)
        {
            E_ADDR val = *arr;

            if(!val || (val == static_cast<ADDR>(-1)))
                continue;

            address_t address = this->addressof(arr);
            //this->m_document->pointer(address, SymbolTable::name(address, prefix, SymbolTypes::Pointer), SymbolTypes::Data);
            //this->m_document->function(val);
        }
    }
}

template<size_t b, endianness_t e> void ELFLoader<b, e>::loadSymbols(const SHDR& shdr)
{
    offset_t offset = shdr.sh_offset, endoffset = offset + shdr.sh_size;
    const SHDR& shstr = shdr.sh_link ? this->m_shdr[shdr.sh_link] : ELF_STRING_TABLE;

    for(u64 idx = 0; offset < endoffset; idx++)
    {
        bool isrelocated = false;
        SYM* sym = ELF_POINTER(SYM, offset);
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

template<size_t b, endianness_t e> void ELFLoader<b, e>::parseSegments()
{
    for(u64 i = 0; i < this->m_header->e_shnum; i++)
    {
        const SHDR& shdr = this->m_shdr[i];

        if(shdr.sh_offset && ((shdr.sh_type == SHT_SYMTAB) || (shdr.sh_type == SHT_DYNSYM)))
        {
            const SHDR& shstr = ELF_STRING_TABLE;
            REDasm::log("Section" + REDasm::quoted(ELF_STRING(&shstr, shdr.sh_name)) + " contains a "
                        "symbol table @ offset " + REDasm::hex(shdr.sh_offset));

            this->loadSymbols(shdr);
        }
    }
}

} // namespace REDasm
