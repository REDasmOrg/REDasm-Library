#include "instructioncache.h"
#include <fstream>
#include <iomanip>
#include "../../libs/cereal/archives/binary.hpp"
#include <redasm/support/utils.h>

#define CACHE_FILE_NAME(x)          ("redasm_cache_" + String::number(x) + ".tmp")

namespace REDasm {

std::unordered_set<String> InstructionCache::m_activenames;

InstructionCache::InstructionCache(): m_filepath(generateFilePath())
{
    m_file.exceptions(std::fstream::failbit);
    m_file.open(m_filepath.c_str(), std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);

    if(!m_file.is_open())
        r_ctx->log("WARNING: Cannot create file " + m_filepath.quoted() + ", caching is disabled");
}

InstructionCache::~InstructionCache()
{
    m_activenames.erase(m_filepath);
    m_cache.clear();
    m_offsets.clear();

    if(!m_file.is_open())
        return;

    m_file.close();
    std::remove(m_filepath.c_str());
}

size_t InstructionCache::size() const { return m_offsets.size(); }
CachedInstruction InstructionCache::find(address_t address) { auto it = m_cache.find(address); return (it != m_cache.end()) ? this->deserialize(address) : it->second; }

CachedInstruction InstructionCache::allocate(address_t address)
{
    CachedInstruction cachedinstruction(this, new Instruction());
    cachedinstruction->address = address;
    this->serialize(cachedinstruction);
    return cachedinstruction;
}

void InstructionCache::deallocate(const CachedInstruction& instruction)
{
    if(m_file.is_open())
        this->serialize(instruction);

    m_cache.erase(instruction->address);
}

void InstructionCache::serialize(const CachedInstruction& instruction)
{
    if(!m_file.is_open())
    {
        m_cache[instruction->address] = instruction;
        return;
    }

    auto it = m_offsets.find(instruction->address);

    if(it == m_offsets.end())
    {
        m_file.seekp(0, std::ios::end); // Append at end
        m_offsets[instruction->address] = m_file.tellp();
    }
    else
        m_file.seekp(it->second, std::ios::beg);

    cereal::BinaryOutputArchive archive(m_file);
    archive(*instruction.get());
}

CachedInstruction InstructionCache::deserialize(address_t address)
{
    if(!m_file.is_open())
    {
        auto it = m_cache.find(address);
        return (it != m_cache.end()) ? it->second : CachedInstruction();
    }

    auto it = m_offsets.find(address);

    if(it == m_offsets.end())
        return CachedInstruction();

    CachedInstruction cachedinstruction(this, new Instruction());
    m_file.seekg(it->second);

    cereal::BinaryInputArchive archive(m_file);
    archive(*cachedinstruction.get());

    m_cache[cachedinstruction->address] = cachedinstruction;
    return cachedinstruction;
}

String InstructionCache::generateFilePath()
{
    String filepath = Path::create(r_ctx->tempPath(), CACHE_FILE_NAME(0));
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
