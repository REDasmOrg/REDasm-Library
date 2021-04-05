#pragma once

#include <rdapi/types.h>
#include <string>
#include <utility>
#include <vector>
#include "../object.h"

#pragma pack(push, 1)
struct ChunkHeader {
    u32 type;
    u32 checksum;
    u32 length;
    u64 offset;
};
#pragma pack(pop)

typedef std::pair<ChunkHeader, RawData> Chunk;

class SerializerCommon
{
    public:
        SerializerCommon() = default;
        const std::string& lastError() const;

    protected:
        bool setLastError(const std::string& s);
        void clearLastError();

    private:
        std::string m_lasterror;
};
