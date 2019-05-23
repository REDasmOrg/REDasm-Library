#pragma once

#include "../../redasm.h"
#include "pe_constants.h"

#define IMAGE_FIRST_SECTION(ntheaders) reinterpret_cast<ImageSectionHeader*>(reinterpret_cast<size_t>(ntheaders) + \
                                                                             ntheaders->FileHeader.SizeOfOptionalHeader + 0x18)

namespace REDasm {

struct ImageDosHeader
{
    u16 e_magic, e_cblp, e_cp, e_crlc, e_cparhdr;
    u16 e_minalloc, e_maxalloc;
    u16 e_ss, e_sp, e_csum, e_ip, e_cs;
    u16 e_lfarlc, e_ovno, e_res[4];
    u16 e_oemid, e_oeminfo, e_res2[10];
    u32 e_lfanew;
};

struct ImageFileHeader
{
    u16 Machine, NumberOfSections;
    u32 TimeDateStamp, PointerToSymbolTable, NumberOfSymbols;
    u16 SizeOfOptionalHeader, Characteristics;
};

struct ImageDataDirectory { u32 VirtualAddress, Size; };

struct ImageOptionalHeader32
{
    u16 Magic;
    u8 MajorLinkerVersion, MinorLinkerVersion;
    u32 SizeOfCode, SizeOfInitializedData, SizeOfUninitializedData;
    u32 AddressOfEntryPoint, BaseOfCode, BaseOfData, ImageBase;
    u32 SectionAlignment, FileAlignment;
    u16 MajorOperatingSystemVersion, MinorOperatingSystemVersion;
    u16 MajorImageVersion, MinorImageVersion;
    u16 MajorSubsystemVersion, MinorSubsystemVersion;
    u32 Win32VersionValue, SizeOfImage, SizeOfHeaders, CheckSum;
    u16 Subsystem, DllCharacteristics;
    u32 SizeOfStackReserve, SizeOfStackCommit;
    u32 SizeOfHeapReserve, SizeOfHeapCommit;
    u32 LoaderFlags, NumberOfRvaAndSizes;
    ImageDataDirectory DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
};

struct ImageOptionalHeader64
{
    u16 Magic;
    u8 MajorLinkerVersion, MinorLinkerVersion;
    u32 SizeOfCode, SizeOfInitializedData, SizeOfUninitializedData;
    u32 AddressOfEntryPoint, BaseOfCode;
    u64 ImageBase;
    u32 SectionAlignment, FileAlignment;
    u16 MajorOperatingSystemVersion, MinorOperatingSystemVersion;
    u16 MajorImageVersion, MinorImageVersion;
    u16 MajorSubsystemVersion, MinorSubsystemVersion;
    u32 Win32VersionValue, SizeOfImage, SizeOfHeaders;
    u32 CheckSum;
    u16 Subsystem, DllCharacteristics;
    u64 SizeOfStackReserve, SizeOfStackCommit;
    u64 SizeOfHeapReserve, SizeOfHeapCommit;
    u32 LoaderFlags, NumberOfRvaAndSizes;
    ImageDataDirectory DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
};

struct ImageNtHeaders
{
    u32 Signature;
    ImageFileHeader FileHeader;

    union
    {
        u16 OptionalHeaderMagic;
        ImageOptionalHeader32 OptionalHeader32;
        ImageOptionalHeader64 OptionalHeader64;
    };
};

struct ImageSectionHeader
{
    u8 Name[IMAGE_SIZEOF_SHORT_NAME];
    union { u32 PhysicalAddress, VirtualSize; } Misc;

    u32 VirtualAddress, SizeOfRawData, PointerToRawData;
    u32 PointerToRelocations, PointerToLinenumbers;
    u16 NumberOfRelocations, NumberOfLinenumbers;
    u32 Characteristics;
};

struct ImageExportDirectory
{
    u32 Characteristics, TimeDateStamp;
    u16 MajorVersion, MinorVersion;
    u32 Name, Base;
    u32 NumberOfFunctions, NumberOfNames;
    u32 AddressOfFunctions, AddressOfNames, AddressOfNameOrdinals;
};

struct ImageDebugDirectory
{
    u32 Characteristics, TimeDateStamp;
    u16 MajorVersion, MinorVersion;
    u32 Type, SizeOfData;
    u32 AddressOfRawData, PointerToRawData;
};

struct ImageBaseRelocation { u32 VirtualAddress, SizeOfBlock; /* u16 TypeOffset[1]; */ };

struct ImageResourceDirectory
{
    u32 Characteristics, TimeDateStamp;
    u16 MajorVersion, MinorVersion;
    u16 NumberOfNamedEntries, NumberOfIdEntries;
    // ImageResourceDirectoryEntry DirectoryEntries[];
};

struct ImageResourceDirectoryEntry
{
    union
    {
        struct { u32 NameOffset:31, NameIsString:1; };
        u32 Name;
        u16 Id;
    };

    union
    {
        u32 OffsetToData;
        struct { u32 OffsetToDirectory:31, DataIsDirectory:1; };
    };
};

struct ImageResourceDirStringU { u16 Length; char NameString[1]; };
struct ImageResourceDataEntry { u32 OffsetToData, Size, CodePage, Reserved; };

struct ImageImportDescriptor
{
    union { u32 Characteristics, OriginalFirstThunk; };

    u32 TimeDateStamp, ForwarderChain;
    u32 Name, FirstThunk;
};

struct ImageImportByName { u16 Hint; u8 Name[1]; };

typedef u32 ImageThunkData32;
typedef u64 ImageThunkData64;

template<typename T> struct ImageTlsDirectory
{
    T StartAddressOfRawData;
    T EndAddressOfRawData;
    T AddressOfIndex;
    T AddressOfCallBacks;
    u32 SizeOfZeroFill;
    u32 Characteristics;
};

typedef ImageTlsDirectory<u32> ImageTlsDirectory32;
typedef ImageTlsDirectory<u64> ImageTlsDirectory64;

struct ImageLoadConfigDirectory32
{
    u32 Size, TimeDateStamp;
    u16 MajorVersion, MinorVersion;
    u32 GlobalFlagsClear, GlobalFlagsSet, CriticalSectionDefaultTimeout;
    u32 DeCommitFreeBlockThreshold, DeCommitTotalFreeThreshold;
    u32 LockPrefixTable;             // VA
    u32 MaximumAllocationSize, VirtualMemoryThreshold;
    u32 ProcessHeapFlags, ProcessAffinityMask;
    u16 CSDVersion, Reserved1;
    u32 EditList;                    // VA
    u32 SecurityCookie;              // VA
    u32 SEHandlerTable;              // VA
    u32 SEHandlerCount;
    u32 GuardCFCheckFunctionPointer; // VA
    u32 Reserved2;
    u32 GuardCFFunctionTable;        // VA
    u32 GuardCFFunctionCount, GuardFlags;
};

struct ImageLoadConfigDirectory64
{
    u32 Size, TimeDateStamp;
    u16 MajorVersion, MinorVersion;
    u32 GlobalFlagsClear, GlobalFlagsSet, CriticalSectionDefaultTimeout;
    u64 DeCommitFreeBlockThreshold, DeCommitTotalFreeThreshold;
    u64 LockPrefixTable;             // VA
    u64 MaximumAllocationSize, VirtualMemoryThreshold;
    u64 ProcessAffinityMask, ProcessHeapFlags;
    u16 CSDVersion, Reserved1;
    u64 EditList;                    // VA
    u64 SecurityCookie;              // VA
    u64 SEHandlerTable;              // VA
    u64 SEHandlerCount;
    u64 GuardCFCheckFunctionPointer; // VA
    u64 Reserved2;
    u64 GuardCFFunctionTable;        // VA
    u64 GuardCFFunctionCount;
    u32 GuardFlags;
};

struct ImageRuntimeFunctionEntry { u32 BeginAddress, EndAddress, UnwindInfoAddress; };

union UnwindCodeU
{
    struct {
        u8 CodeOffset;
        u8 UnwindOp : 4;
        u8 OpInfo : 4;
    };

    u16 FrameOffset;
};

struct UnwindInfo
{
    u8 Version : 3;
    u8 Flags : 5;
    u8 SizeOfProlog;
    u8 CountOfCodes;
    u8 FrameRegister : 4;
    u8 FrameOffset : 4;
    UnwindCodeU UnwindCode[1];
};

} // namespace REDasm
