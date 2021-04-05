#include "reader.h"
#include <rdcore/support/endian.h>
#include <rdcore/support/hash.h>
#include <fstream>

bool SerializerReader::read(const std::string& filepath)
{
    std::ifstream fs(filepath, std::fstream::binary | std::fstream::ate);
    if(!fs.is_open()) return this->setLastError("Cannot read " + filepath);

    // Get chunks count
    u64 chunks;
    fs.seekg(-sizeof(u64), std::fstream::cur);
    fs.read(reinterpret_cast<char*>(&chunks), sizeof(u64));
    chunks = Endian::tolittleendian32(chunks);

    // Rewind stream
    fs.seekg(0, std::fstream::beg);

    // Read Chunks
    for(u64 i = 0; !fs.eof() && (i < chunks); i++)
    {
        ChunkHeader chdr;
        fs.read(reinterpret_cast<char*>(&chdr), sizeof(ChunkHeader));

        std::string type = SerializerReader::mktype(chdr.type);
        auto pos = fs.tellg();
        RawData chdata(chdr.length);

        fs.seekg(chdr.offset,std::fstream::beg);
        fs.read(reinterpret_cast<char*>(chdata.data()), chdr.length);

        if(Hash::crc32(chdata.data(), chdata.size()) != chdr.checksum)
            return this->setLastError("Invalid Checksum for chunk " + type);

        m_chunks[type].push_back(chdata);
        fs.seekg(pos);
    }

    return true;
}

std::optional<SerializerReader::Result> SerializerReader::find(const std::string& type) const
{
    auto it = m_chunks.find(type);
    return (it != m_chunks.end()) ? std::make_optional(it->second) : std::nullopt;
}

std::string SerializerReader::mktype(u32 type)
{
    type = Endian::tolittleendian32(type);
    return std::string(reinterpret_cast<const char*>(&type), sizeof(u32));
}
