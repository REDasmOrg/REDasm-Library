#include "elf.h"

namespace REDasm {

Elf32Format::Elf32Format(AbstractBuffer *buffer): ElfFormat<ELF_PARAMS(32)>(buffer) { }
u64 Elf32Format::relocationSymbol(const Elf32_Rel *rel) const { return ELF32_R_SYM(rel->r_info); }

Elf64Format::Elf64Format(AbstractBuffer *buffer): ElfFormat<ELF_PARAMS(64)>(buffer) { }
u64 Elf64Format::relocationSymbol(const Elf64_Rel *rel) const { return ELF64_R_SYM(rel->r_info); }

}
