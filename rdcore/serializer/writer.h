#pragma once

#include <list>
#include "common.h"

class SerializerWriter: public SerializerCommon
{
    private:

    public:
        SerializerWriter() = default;
        bool push(const std::string& id, const RawData& chunkdata);
        bool save(const std::string& filepath);

    public:
        template<typename T> bool push(const std::string& id, const T* t);

    private:
        static u32 mktype(const std::string& s);

    private:
        std::list<Chunk> m_chunks;
};

template<typename T>
bool SerializerWriter::push(const std::string& id, const T* t) {
    if(!t) return false;
    auto* p = reinterpret_cast<const u8*>(t);

    RawData chunkdata(sizeof(T));
    chunkdata.assign(p, p + sizeof(T));
    return this->push(id, chunkdata);
}
