#ifndef ELF_H
#define ELF_H

#include "../../plugins/plugins.h"
#include "elf32_header.h"
#include "elf64_header.h"
#include "elf_analyzer.h"

#define ELF_T(bits, t) Elf ## bits ## _ ## t
#define ELF_PARAMS_T typename EHDR, typename SHDR, typename PHDR, typename SYM, typename REL, typename RELA
#define ELF_PARAMS_D EHDR, SHDR, PHDR, SYM, REL, RELA
#define ELF_PARAMS(bits) ELF_T(bits, Ehdr), ELF_T(bits, Shdr), ELF_T(bits, Phdr), ELF_T(bits, Sym), ELF_T(bits, Rel), ELF_T(bits, Rela)

#define POINTER(T, offset) FormatPluginT<EHDR>::template pointer<T>(offset)
#define ELF_STRING_TABLE this->m_shdr[this->m_format->e_shstrndx];
#define ELF_STRING(shdr, offset) POINTER(const char, (shdr)->sh_offset + offset)

namespace REDasm {

template<ELF_PARAMS_T> class ElfFormat: public FormatPluginT<EHDR>
{
    DEFINE_FORMAT_PLUGIN_TEST(EHDR)

    public:
        ElfFormat(Buffer& buffer);
        virtual std::string name() const { return "ELF" + std::to_string(this->bits()) + " Format"; }
        virtual std::string assembler() const;
        virtual u32 bits() const;
        virtual void load();
        virtual Analyzer* createAnalyzer(DisassemblerAPI *disassembler, const SignatureFiles &signatures) const;

    protected:
        virtual u64 relocationSymbol(const REL* rel) const = 0;

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

class Elf32Format: public ElfFormat<ELF_PARAMS(32)>
{
    public:
        Elf32Format(Buffer& buffer);

    protected:
        virtual u64 relocationSymbol(const Elf32_Rel* rel) const;
};

class Elf64Format: public ElfFormat<ELF_PARAMS(64)>
{
    public:
        Elf64Format(Buffer& buffer);

    protected:
        virtual u64 relocationSymbol(const Elf64_Rel* rel) const;
};

DECLARE_FORMAT_PLUGIN(Elf32Format, elf32)
DECLARE_FORMAT_PLUGIN(Elf64Format, elf64)

}

#include "elf_impl.h"

#endif // ELF_H
