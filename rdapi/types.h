#pragma once

#include <stdint.h>
#include <stddef.h>
#include "macros.h"

#define DEFAULT_NAME_SIZE        32

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef u64 rd_location;
typedef rd_location rd_address;
typedef rd_location rd_offset;

typedef u16 rd_type;
typedef u16 rd_flag;
typedef u16 rd_endianness;

#ifdef __cplusplus
    #define RD_NVAL static_cast<size_t>(-1)
#else
    #define RD_NVAL (size_t)(-1)
#endif

#define PRIVATE_RD_USERDATA_FIELDS \
    void* userdata; \
    void* p_data; \
    intptr_t i_data; \
    uintptr_t u_data; \
    const char* s_data;

#define RD_USERDATA_FIELD union { PRIVATE_RD_USERDATA_FIELDS };

typedef struct RDLocation {
    union {
        rd_location value;
        rd_offset offset;
        rd_address address;
    };

    bool valid;
} RDLocation;

enum RDSegmentFlags {
    SegmentFlags_None,
    SegmentFlags_Code = (1 << 0),
    SegmentFlags_Data = (1 << 1),
    SegmentFlags_Bss  = (1 << 2),

    SegmentFlags_CodeData  = SegmentFlags_Code | SegmentFlags_Data,
};

enum RDReferenceFlags {
    ReferenceFlags_None,    // Internal use
    ReferenceFlags_Direct   = (1 << 0),
    ReferenceFlags_Indirect = (1 << 1),
    ReferenceFlags_Manual   = (1 << 2),
};

typedef struct RDReference {
    rd_address address;
    rd_flag flags;
} RDReference;

typedef struct RDSegment {
    char name[DEFAULT_NAME_SIZE];
    rd_offset offset;    // [offset, endoffset)
    rd_offset endoffset;
    rd_address address;  // [address, endaddress)
    rd_address endaddress;
    rd_flag flags;

    RD_USERDATA_FIELD
} RDSegment;

typedef struct RDAnalysisStatus {
    // General
    const char* filepath;
    const char* assembler;
    const char* loader;
    size_t filesize;
    bool busy;

    // Time
    u64 analysisstart;
    u64 analysisend;

    // Steps
    const char* const* stepslist;
    size_t stepscount;
    size_t stepscurrent;

    // Analyzers
    const char* const* analyzerslist;
    size_t* analyzersdone;
    size_t analyzerscount;
    size_t analyzerscurrent;

    // Stats
    size_t segmentscount;
    size_t functionscount;
    size_t symbolscount;
    int segmentsdiff;
    int functionsdiff;
    int symbolsdiff;
} RDAnalysisStatus;

RD_API_EXPORT size_t RDSegment_RawSize(const RDSegment* s);
RD_API_EXPORT size_t RDSegment_Size(const RDSegment* s);
RD_API_EXPORT bool RDSegment_ContainsAddress(const RDSegment* s, rd_address address);
RD_API_EXPORT bool RDSegment_ContainsOffset(const RDSegment* s, rd_offset offset);
