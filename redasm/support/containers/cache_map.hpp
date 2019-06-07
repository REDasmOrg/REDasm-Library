#pragma once

#define CACHE_FILE_NAME(x)     ("redasm_cache_" + std::to_string(x) + ".tmp")

#include "cache_map.h"
#include "../../context.h"
#include "../serializer.h"
#include "../utils.h"
#include "../path.h"
#include <ios>

namespace REDasm {

template<typename Key, typename Value> std::unordered_set<std::string> cache_map<Key, Value>::m_activenames;

template<typename Key, typename Value> cache_map<Key, Value>::cache_map(): m_filepath(generateFilePath())
{
    m_file.exceptions(std::fstream::failbit);
    m_file.open(m_filepath, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);

    if(!m_file.is_open())
        r_ctx->log("Cannot write cache @ " + Utils::quoted(m_filepath));
}

template<typename Key, typename Value> cache_map<Key, Value>::~cache_map()
{
    m_activenames.erase(m_filepath);

    if(!m_file.is_open())
        return;

    m_file.close();
    std::remove(m_filepath.c_str());
}

template<typename Key, typename Value> size_t cache_map<Key, Value>::size() const { return m_offsets.size(); }

template<typename Key, typename Value> void cache_map<Key, Value>::commit(const Key& key, const Value &value)
{
    m_file.seekp(0, std::ios::end); // Ignore old key -> value reference, if any
    m_offsets[key] = m_file.tellp();

    Serializer<Value>::write(m_file, value);
}

template<typename Key, typename Value> void cache_map<Key, Value>::erase(const cache_map<Key, Value>::iterator &it)
{
    auto oit = m_offsets.find(it.key);

    if(oit == m_offsets.end())
        return;

    m_offsets.erase(oit);
}

template<typename Key, typename Value> Value cache_map<Key, Value>::value(const Key &key)
{
    auto it = m_offsets.find(key);

    if(it == m_offsets.end())
        return Value();

    Value value;

    auto v = m_file.rdstate();
    m_file.seekg(it->second, std::ios::beg);
    v = m_file.rdstate();

    Serializer<Value>::read(m_file, value);
    return value;
}

template<typename Key, typename Value> Value cache_map<Key, Value>::operator[](const Key& key) { return this->value(key); }

template<typename Key, typename Value> std::string cache_map<Key, Value>::generateFilePath()
{
    std::string filepath = Path::create(r_ctx->tempPath(), CACHE_FILE_NAME(0));
    auto it = m_activenames.find(filepath);

    for(size_t i = 1; it != m_activenames.end(); i++)
    {
        filepath = Path::create(r_ctx->tempPath(), CACHE_FILE_NAME(i));
        it = m_activenames.find(filepath);
    }

    m_activenames.insert(filepath);
    return filepath;
}

} // namespace REDasm
