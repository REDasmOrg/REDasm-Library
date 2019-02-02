#ifndef ELF_H
#define ELF_H

#include "../../plugins/plugins.h"
#include "elf_header.h"
#include "elf_analyzer.h"

#define ELF_POINTER(T, offset) FormatPluginT<EHDR>::template pointer<T>(offset)
#define ELF_STRING_TABLE this->m_shdr[this->m_format->e_shstrndx];
#define ELF_STRING(shdr, offset) ELF_POINTER(const char, (shdr)->sh_offset + offset)

namespace REDasm {

template<size_t b, endianness_t e> class ElfFormat: public FormatPluginT< Elf_Ehdr<b, e> >
{
    public:
        typedef Elf_Ehdr<b, e> EHDR;
        typedef Elf_Shdr<b, e> SHDR;
        typedef Elf_Rel<b, e> REL;
        typedef Elf_Rela<b, e> RELA;
        typedef typename std::conditional<b == 64, Elf64_Phdr<e>, Elf32_Phdr<e> >::type PHDR;
        typedef typename std::conditional<b == 64, Elf64_Sym<e>, Elf32_Sym<e> >::type SYM;

    DEFINE_FORMAT_PLUGIN_TEST(ELF_ARG(Elf_Ehdr<b, e>))

    public:
        ElfFormat(AbstractBuffer* buffer);
        virtual std::string name() const;
        virtual std::string assembler() const;
        virtual endianness_t endianness() const;
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

typedef ElfFormat<32, Endianness::LittleEndian> Elf32LEFormat;
typedef ElfFormat<32, Endianness::BigEndian> Elf32BEFormat;
typedef ElfFormat<64, Endianness::LittleEndian> Elf64LEFormat;
typedef ElfFormat<64, Endianness::BigEndian> Elf64BEFormat;

DECLARE_FORMAT_PLUGIN(Elf32LEFormat, elf32le)
DECLARE_FORMAT_PLUGIN(Elf32BEFormat, elf32be)
DECLARE_FORMAT_PLUGIN(Elf64LEFormat, elf64le)
DECLARE_FORMAT_PLUGIN(Elf64BEFormat, elf64be)

}

#include "elf_impl.h"

#endif // ELF_H
