#pragma once

#include <functional>
#include <algorithm>
#include <list>
#include "common.h"

class SerializerWriter: public SerializerCommon
{
    private:

    public:
        SerializerWriter() = default;
        bool push(const std::string& id, const RawData& chdata);
        bool save(const std::string& filepath);

    public:
        template<typename Iterator> bool push(const std::string& id, Iterator begin, Iterator end, const std::function<void(const typename Iterator::value_type*, typename Iterator::value_type*)>& cb);
        template<typename T> bool push(const std::string& id, const T* t);

    private:
        static bool needsCompression(u32 size);
        static u32 mktype(const std::string& s);

    private:
        std::list<Chunk> m_chunks;
};

template<typename Iterator>
bool SerializerWriter::push(const std::string& id, Iterator begin, Iterator end, const std::function<void(const typename Iterator::value_type*, typename Iterator::value_type*)>& cb) {
    if(id.empty()) return false;

    RawData data;

    for(auto it = begin; it != end; it++) {
        typename Iterator::value_type t;
        cb(std::addressof(*it), &t);

        data.resize(data.size() + sizeof(t));
        std::copy_n(reinterpret_cast<u8*>(&t), sizeof(t), data.end() - sizeof(t));
    }

    return this->push(id, data);
}

template<typename T>
bool SerializerWriter::push(const std::string& id, const T* t) {
    if(!t) return false;
    auto* p = reinterpret_cast<const u8*>(t);

    RawData chunkdata(sizeof(T));
    chunkdata.assign(p, p + sizeof(T));
    return this->push(id, chunkdata);
}
