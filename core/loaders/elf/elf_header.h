#pragma once

#include "../../redasm.h"
#include "elf_common.h"

namespace REDasm {

#define ELF_ARG(...)    __VA_ARGS__
#define ELF32_R_SYM(i) ((i) >> 8)
#define ELF64_R_SYM(i) ((i) >> 32)

#define Elf_SInteger(bits, endianness) endian_type(type_from_bits(bits,signed_type), endianness)
#define Elf_Integer(bits, endianness)  endian_type(type_from_bits(bits,unsigned_type), endianness)
#define Elf_Addr(bits, endianness)     Elf_Integer(bits, endianness)
#define Elf_Off(bits, endianness)      Elf_Integer(bits, endianness)

template<size_t bits, endianness_t endianness> struct Elf_Ehdr
{
    unsigned char e_ident[EI_NIDENT];
    endian_type(u16, endianness) e_type;
    endian_type(u16, endianness) e_machine;
    endian_type(u16, endianness) e_version;
    Elf_Addr(bits, endianness) e_entry;
    Elf_Off(bits, endianness) e_phoff;
    Elf_Off(bits, endianness) e_shoff;
    endian_type(u32, endianness) e_flags;
    endian_type(u16, endianness) e_ehsize;
    endian_type(u16, endianness) e_phentsize;
    endian_type(u16, endianness) e_phnum;
    endian_type(u16, endianness) e_shentsize;
    endian_type(u16, endianness) e_shnum;
    endian_type(u16, endianness) e_shstrndx;
};

template<size_t bits, endianness_t endianness> struct Elf_Shdr
{
    endian_type(u32, endianness) sh_name;
    endian_type(u32, endianness) sh_type;
    Elf_Integer(bits, endianness) sh_flags;
    Elf_Addr(bits, endianness) sh_addr;
    Elf_Off(bits, endianness) sh_offset;
    Elf_Integer(bits, endianness) sh_size;
    endian_type(u32, endianness) sh_link;
    endian_type(u32, endianness) sh_info;
    Elf_Integer(bits, endianness) sh_addralign;
    Elf_Integer(bits, endianness) sh_endsize;
};

template<size_t bits, endianness_t endianness> struct Elf_Rel
{
    Elf_Addr(bits, endianness) r_offset;
    Elf_Integer(bits, endianness) r_info;
};

template<size_t bits, endianness_t endianness> struct Elf_Rela
{
    Elf_Addr(bits, endianness) r_offset;
    Elf_Integer(bits, endianness) r_info;
    Elf_SInteger(bits, endianness) r_addend;
};

template<endianness_t endianness> struct Elf32_Phdr
{
    endian_type(u32, endianness) p_type;
    Elf_Off(32, endianness) p_offset;
    Elf_Addr(32, endianness) p_vaddr;
    Elf_Addr(32, endianness) p_paddr;
    endian_type(u32, endianness) p_filesz;
    endian_type(u32, endianness) p_memsz;
    endian_type(u32, endianness) p_flags;
    endian_type(u32, endianness) p_align;
};

template<endianness_t endianness> struct Elf64_Phdr
{
    endian_type(u32, endianness) p_type;
    endian_type(u32, endianness) p_flags;
    Elf_Off(64, endianness) p_offset;
    Elf_Addr(64, endianness) p_vaddr;
    Elf_Addr(64, endianness) p_paddr;
    endian_type(u64, endianness) p_filesz;
    endian_type(u64, endianness) p_memsz;
    endian_type(u64, endianness) p_align;
};

template<endianness_t endianness> struct Elf32_Sym
{
    endian_type(u32, endianness) st_name;
    Elf_Addr(32, endianness) st_value;
    endian_type(u32, endianness) st_size;
    u8 st_info;
    u8 st_other;
    endian_type(u16, endianness) st_shndx;
};

template<endianness_t endianness> struct Elf64_Sym
{
    endian_type(u32, endianness) st_name;
    u8 st_info;
    u8 st_other;
    endian_type(u16, endianness) st_shndx;
    Elf_Addr(64, endianness) st_value;
    endian_type(u64, endianness) st_size;
};

} // namespace REDasm
