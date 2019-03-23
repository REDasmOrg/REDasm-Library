#ifndef MSCOFF_HEADER_H
#define MSCOFF_HEADER_H

#define MSCOFF_SIGNATURE       "!<arch>\n"
#define MSCOFF_SIGNATURE_SIZE  8

#include "../../redasm.h"

namespace REDasm {

struct ImageArchiveMemberHeader {
    char name[16];
    char date[12];
    char ownerid[6];
    char groupid[6];
    char mode[8];
    char size[10];
    char endheader[2];
};

struct ImageArchiveHeader {
    char signature[MSCOFF_SIGNATURE_SIZE];
    ImageArchiveMemberHeader first;
};

struct ImageFirstLinker {
    u32 nsymbols;
    u32 offsets[1];
};

struct ImageSecondLinker {
    u32 nmembers;
    u32 offsets[1];
};

struct ImageRelocation {
    u32 VirtualAddress;
    u32 SymbolTableIndex;
    u16 Type;
};

} // namespace REDasm

#endif // ARCHIVIER_HEADER_H
