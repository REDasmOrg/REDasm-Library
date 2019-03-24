#ifndef PE_CONSTANTS_H
#define PE_CONSTANTS_H

#include "../../redasm/types/base_types.h"

// Signatures
#define IMAGE_DOS_SIGNATURE                               0x5A4D
#define IMAGE_NT_SIGNATURE                            0x00004550
#define IMAGE_NT_OPTIONAL_HDR32_MAGIC                      0x10b
#define IMAGE_NT_OPTIONAL_HDR64_MAGIC                      0x20b
#define IMAGE_ROM_OPTIONAL_HDR_MAGIC                       0x107

// Architecture
#define IMAGE_FILE_MACHINE_AM33                            0x1d3
#define IMAGE_FILE_MACHINE_AMD64                          0x8664
#define IMAGE_FILE_MACHINE_ARM                             0x1c0
#define IMAGE_FILE_MACHINE_EBC                             0xebc
#define IMAGE_FILE_MACHINE_I386                            0x14c
#define IMAGE_FILE_MACHINE_IA64                            0x200
#define IMAGE_FILE_MACHINE_M32R                           0x9041
#define IMAGE_FILE_MACHINE_MIPS16                          0x266
#define IMAGE_FILE_MACHINE_MIPSFPU                         0x366
#define IMAGE_FILE_MACHINE_MIPSFPU16                       0x466
#define IMAGE_FILE_MACHINE_POWERPC                         0x1f0
#define IMAGE_FILE_MACHINE_POWERPCFP                       0x1f1
#define IMAGE_FILE_MACHINE_R4000                           0x166
#define IMAGE_FILE_MACHINE_SH3                             0x1a2
#define IMAGE_FILE_MACHINE_SH3E                           0x01a4
#define IMAGE_FILE_MACHINE_SH3DSP                          0x1a3
#define IMAGE_FILE_MACHINE_SH4                             0x1a6
#define IMAGE_FILE_MACHINE_SH5                             0x1a8
#define IMAGE_FILE_MACHINE_THUMB                           0x1c2
#define IMAGE_FILE_MACHINE_WCEMIPSV2                       0x169
#define IMAGE_FILE_MACHINE_R3000                           0x162
#define IMAGE_FILE_MACHINE_R10000                          0x168
#define IMAGE_FILE_MACHINE_ALPHA                           0x184
#define IMAGE_FILE_MACHINE_ALPHA64                        0x0284
#define IMAGE_FILE_MACHINE_AXP64      IMAGE_FILE_MACHINE_ALPHA64
#define IMAGE_FILE_MACHINE_CEE                            0xC0EE
#define IMAGE_FILE_MACHINE_TRICORE                        0x0520
#define IMAGE_FILE_MACHINE_CEF                            0x0CEF

// Sections
#define IMAGE_SIZEOF_SHORT_NAME                                8

#define IMAGE_SCN_MEM_DISCARDABLE                     0x02000000
#define IMAGE_SCN_MEM_EXECUTE                         0x20000000
#define IMAGE_SCN_MEM_READ                            0x40000000
#define IMAGE_SCN_MEM_WRITE                           0x80000000

#define IMAGE_SCN_CNT_CODE                            0x00000020  // Section contains code
#define IMAGE_SCN_CNT_INITIALIZED_DATA                0x00000040  // Section contains initialized data
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA              0x00000080  // Section contains uninitialized data

// Optional Header -> DllCharacteristics
#define IMAGE_DLL_CHARACTERISTICS_DYNAMIC_BASE          0x0040
#define IMAGE_DLL_CHARACTERISTICS_FORCE_INTEGRITY       0x0080
#define IMAGE_DLL_CHARACTERISTICS_NX_COMPAT             0x0100
#define IMAGE_DLL_CHARACTERISTICS_NO_ISOLATION          0x0200
#define IMAGE_DLL_CHARACTERISTICS_NO_SEH                0x0400
#define IMAGE_DLL_CHARACTERISTICS_NO_BIND               0x0800
#define IMAGE_DLL_CHARACTERISTICS_WDM_DRIVER            0x2000
#define IMAGE_DLL_CHARACTERISTICS_TERMINAL_SERVER_AWARE 0x8000

// Data Directory
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES                      16

#define IMAGE_DIRECTORY_ENTRY_EXPORT                                              0  // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT                                              1  // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE                                            2  // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION                                           3  // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY                                            4  // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC                                           5  // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG                                               6  // Debug Directory
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE                                        7  // Architecture Specific Data
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR                                           8  // RVA of GP
#define IMAGE_DIRECTORY_ENTRY_TLS                                                 9  // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG                                        10  // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT                                       11  // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT                                                12  // Import Address Table
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT                                       13  // Delay Load Import Descriptors
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR                                     14  // COM Runtime descriptor
#define IMAGE_DIRECTORY_ENTRY_DOTNET           IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR  // .NET descriptor

// Import Table
#define IMAGE_ORDINAL_FLAG64               0x8000000000000000ULL
#define IMAGE_ORDINAL_FLAG32                          0x80000000

// Resource Directory
#define IMAGE_RESOURCE_NAME_IS_STRING                 0x80000000
#define IMAGE_RESOURCE_DATA_IS_DIRECTORY              0x80000000

// Relocations Directory
#define IMAGE_REL_BASED_ABSOLUTE                               0
#define IMAGE_REL_BASED_HIGH                                   1
#define IMAGE_REL_BASED_LOW                                    2
#define IMAGE_REL_BASED_HIGHLOW                                3
#define IMAGE_REL_BASED_HIGHADJ                                4
#define IMAGE_REL_BASED_MIPS_JMPADDR                           5
#define IMAGE_REL_BASED_SECTION                                6
#define IMAGE_REL_BASED_REL32                                  7

#define IMAGE_REL_BASED_MIPS_JMPADDR16                         9
#define IMAGE_REL_BASED_IA64_IMM64                             9
#define IMAGE_REL_BASED_DIR64                                 10
#define IMAGE_REL_BASED_HIGH3ADJ                              11

#define IMAGE_REL_I386_ABSOLUTE  0x0000
#define IMAGE_REL_I386_DIR16     0x0001
#define IMAGE_REL_I386_REL16     0x0002
#define IMAGE_REL_I386_DIR32     0x0006
#define IMAGE_REL_I386_DIR32NB   0x0007
#define IMAGE_REL_I386_SEG12     0x0009
#define IMAGE_REL_I386_SECTION   0x000A
#define IMAGE_REL_I386_SECREL    0x000B
#define IMAGE_REL_I386_TOKEN     0x000C
#define IMAGE_REL_I386_SECREL7   0x000D
#define IMAGE_REL_I386_REL32     0x0014

#define IMAGE_REL_AMD64_ABSOLUTE 0x0000
#define IMAGE_REL_AMD64_ADDR64   0x0001
#define IMAGE_REL_AMD64_ADDR32   0x0002
#define IMAGE_REL_AMD64_ADDR32NB 0x0003
#define IMAGE_REL_AMD64_REL32    0x0004
#define IMAGE_REL_AMD64_REL32_1  0x0005
#define IMAGE_REL_AMD64_REL32_2  0x0006
#define IMAGE_REL_AMD64_REL32_3  0x0007
#define IMAGE_REL_AMD64_REL32_4  0x0008
#define IMAGE_REL_AMD64_REL32_5  0x0009
#define IMAGE_REL_AMD64_SECTION  0x000A
#define IMAGE_REL_AMD64_SECREL   0x000B
#define IMAGE_REL_AMD64_SECREL7  0x000C
#define IMAGE_REL_AMD64_TOKEN    0x000D
#define IMAGE_REL_AMD64_SREL32   0x000E
#define IMAGE_REL_AMD64_PAIR     0x000F
#define IMAGE_REL_AMD64_SSPAN32  0x0010

#define IMAGE_REL_ARM_ABSOLUTE   0x0000
#define IMAGE_REL_ARM_ADDR32     0x0001
#define IMAGE_REL_ARM_ADDR32NB   0x0002
#define IMAGE_REL_ARM_BRANCH24   0x0003
#define IMAGE_REL_ARM_BRANCH11   0x0004
#define IMAGE_REL_ARM_SECTION    0x000e
#define IMAGE_REL_ARM_SECREL     0x000f

// Debug Info
#define IMAGE_DEBUG_TYPE_UNKNOWN                               0
#define IMAGE_DEBUG_TYPE_COFF                                  1
#define IMAGE_DEBUG_TYPE_CODEVIEW                              2
#define IMAGE_DEBUG_TYPE_FPO                                   3
#define IMAGE_DEBUG_TYPE_MISC                                  4
#define IMAGE_DEBUG_TYPE_EXCEPTION                             5
#define IMAGE_DEBUG_TYPE_FIXUP                                 6
#define IMAGE_DEBUG_TYPE_OMAP_TO_SRC                           7
#define IMAGE_DEBUG_TYPE_OMAP_FROM_SRC                         8
#define IMAGE_DEBUG_TYPE_BORLAND                               9
#define IMAGE_DEBUG_TYPE_RESERVED10                           10
#define IMAGE_DEBUG_TYPE_CLSID                                11
#define IMAGE_DEBUG_TYPE_VC_FEATURE                           12
#define IMAGE_DEBUG_TYPE_POGO                                 13
#define IMAGE_DEBUG_TYPE_ILTCG                                14
#define IMAGE_DEBUG_TYPE_MPX                                  15
#define IMAGE_DEBUG_TYPE_REPRO                                16

#define UNW_FLAG_NHANDLER                                      0
#define UNW_FLAG_EHANDLER                                      1
#define UNW_FLAG_UHANDLER                                      2
#define UNW_FLAG_CHAININFO                                     4

#define PE_SECURITY_COOKIE_SYMBOL                             "__security_cookie"

#endif // PE_CONSTANTS_H
