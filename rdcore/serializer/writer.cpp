#include "writer.h"
#include "../support/compression.h"
#include <rdcore/support/endian.h>
#include <rdcore/support/hash.h>
#include <fstream>

bool SerializerWriter::push(const std::string& id, const RawData& chdata)
{
    if(id.empty() || chdata.empty()) return false;

    ChunkHeader chdr;
    chdr.type = SerializerWriter::mktype(id);
    chdr.checksum = Endian::tolittleendian32(Hash::crc32(chdata.data(), chdata.size()));
    chdr.offset = RD_NVAL;

    if(this->needsCompression(chdata.size())) // Compress
    {
        RawData compresseddata;
        compresseddata.reserve(chdata.size()); // Preallocate some space
        Compression::compress(chdata, compresseddata);

        chdr.length = Endian::tolittleendian32(compresseddata.size()),
        m_chunks.push_back({ chdr, compresseddata });
    }
    else
    {
        chdr.length = Endian::tolittleendian32(chdata.size()),
        m_chunks.push_back({ chdr, chdata });
    }

    return true;
}

bool SerializerWriter::save(const std::string& filepath)
{
    if(m_chunks.empty()) return this->setLastError("Nothing to serialize");

    std::ofstream ofs(filepath, std::ofstream::binary);
    if(!ofs.is_open()) return this->setLastError("Cannot write " + filepath);

    // Write Header
    u64 offset = sizeof(ChunkHeader) * m_chunks.size();

    for(auto& [chdr, chdata] : m_chunks)
    {
        chdr.offset = offset;
        ofs.write(reinterpret_cast<const char*>(&chdr), sizeof(ChunkHeader));
        offset += chdata.size();
    }

    // Write Data
    for(auto& [chdr, chdata] : m_chunks)
        ofs.write(reinterpret_cast<const char*>(chdata.data()), chdata.size());

    // Write Chunk Count
    u64 chunks = Endian::tolittleendian64(m_chunks.size());
    ofs.write(reinterpret_cast<const char*>(&chunks), sizeof(u64));
    return true;
}

bool SerializerWriter::needsCompression(u32 size) { return size > sizeof(u64); }

u32 SerializerWriter::mktype(const std::string& s)
{
    u8 val[sizeof(u32)] = { };

    for(size_t i = 0; (i < sizeof(u32)) && (i < s.size()); i++)
        val[i] = static_cast<u8>(s[i]);

    return Endian::tolittleendian32(*reinterpret_cast<u32*>(val));
}
