#ifndef ELFLOADER_H
#define ELFLOADER_H

#include "../../plugins/plugins.h"
#include "elf_header.h"
#include "elf_analyzer.h"

#define ELF_POINTER(T, offset) LoaderPluginT<EHDR>::template pointer<T>(offset)
#define ELF_STRING_TABLE this->m_shdr[this->m_header->e_shstrndx];
#define ELF_STRING(shdr, offset) ELF_POINTER(const char, (shdr)->sh_offset + offset)

namespace REDasm {

template<size_t b, endianness_t e> class ELFLoader: public LoaderPluginT< Elf_Ehdr<b, e> >
{
    public:
        typedef Elf_Ehdr<b, e> EHDR;
        typedef Elf_Shdr<b, e> SHDR;
        typedef Elf_Rel<b, e> REL;
        typedef Elf_Rela<b, e> RELA;
        typedef typename std::conditional<b == 64, Elf64_Phdr<e>, Elf32_Phdr<e> >::type PHDR;
        typedef typename std::conditional<b == 64, Elf64_Sym<e>, Elf32_Sym<e> >::type SYM;

    DECLARE_LOADER_PLUGIN_TEST(ELF_ARG(Elf_Ehdr<b, e>))

    public:
        ELFLoader(AbstractBuffer* buffer);
        virtual std::string assembler() const;
        virtual u32 bits() const;
        virtual void load();
        virtual Analyzer* createAnalyzer(DisassemblerAPI *disassembler, const SignatureFiles &signatures) const;

    protected:
        virtual u64 relocationSymbol(const REL* rel) const;

    private:
        bool relocate(u64 symidx, u64* value) const;
        void loadSymbols(const SHDR& shdr);
        void loadSegments();
        void checkProgramHeader();
        void parseSegments();

    private:
        std::set<std::string> m_skipsections;
        SHDR* m_shdr;
        PHDR* m_phdr;
};

LOADER_INHERIT(ELF32LELoader, (ELFLoader<32, Endianness::LittleEndian>), "ELF Executable (32 bits, LE)")
LOADER_INHERIT(ELF32BELoader, (ELFLoader<32, Endianness::BigEndian>), "ELF Executable (32b bits, BE)")
LOADER_INHERIT(ELF64LELoader, (ELFLoader<64, Endianness::LittleEndian>), "ELF Executable (64 bits, LE)")
LOADER_INHERIT(ELF64BELoader, (ELFLoader<64, Endianness::BigEndian>), "ELF Executable (64 bits, BE)")

DECLARE_LOADER_PLUGIN(ELF32LELoader, elf32le)
DECLARE_LOADER_PLUGIN(ELF32BELoader, elf32be)
DECLARE_LOADER_PLUGIN(ELF64LELoader, elf64le)
DECLARE_LOADER_PLUGIN(ELF64BELoader, elf64be)

}

#include "elf_impl.h"

#endif // ELFLOADER_H
