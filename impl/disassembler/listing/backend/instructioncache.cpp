#include "instructioncache.h"
#include <fstream>
#include <iomanip>
#include <redasm/support/utils.h>
#include "../cachedinstruction_impl.h"

#define CACHE_FILE_NAME(x)          ("redasm_cache_" + String::number(x) + ".tmp")

namespace REDasm {

InstructionCache::InstructionCache(): m_filepath(generateFilePath()), m_lockserialization(false)
{
    m_lmdb.open(m_filepath, MDB_INTEGERKEY | MDB_CREATE, MDB_NOLOCK);
}

InstructionCache::~InstructionCache()
{
    m_lockserialization = true;
    m_loaded.clear();

    m_lmdb.close();
    std::remove(m_filepath.c_str());
}

bool InstructionCache::contains(address_t address) const { return m_cached.find(address) != m_cached.end(); }
CachedInstruction InstructionCache::find(address_t address) { return this->deserialize(address); }

CachedInstruction InstructionCache::allocate()
{
    CachedInstruction ci(this);
    ci.pimpl_p()->m_instruction = std::make_shared<Instruction>();
    return ci;
}

void InstructionCache::store(const CachedInstruction& instruction)
{
    m_loaded[instruction->address] = instruction.pimpl_p()->m_instruction;
    m_cached.insert(instruction->address);
}

void InstructionCache::erase(address_t address) { m_cached.erase(address); }

void InstructionCache::serialize(const InstructionPtr& instruction)
{
    cache_lock lock(m_mutex);
    if(m_lockserialization) return;

    auto it = m_loaded.find(instruction->address);
    if((it != m_loaded.end()) && (it->second.use_count() > 1)) return;

    auto t = m_lmdb.transaction();
    t->putr(instruction->address, static_cast<const InstructionStruct*>(instruction.get()));
    t->commit();
    m_loaded.erase(instruction->address);
}

CachedInstruction InstructionCache::deserialize(address_t address)
{
    cache_lock lock(m_mutex);
    CachedInstruction cachedinstruction(this);
    auto it = m_loaded.find(address);

    if(it == m_loaded.end())
    {
        auto instruction = std::make_shared<Instruction>();

        auto t = m_lmdb.transaction();
        t->get(address, static_cast<InstructionStruct*>(instruction.get()));

        // Reset backend specific fields
        instruction->free = nullptr;
        instruction->puserdata = nullptr;

        m_loaded[address] = instruction;
        cachedinstruction.pimpl_p()->m_instruction = instruction;
    }
    else
        cachedinstruction.pimpl_p()->m_instruction = it->second;

    return cachedinstruction;
}

String InstructionCache::generateFilePath()
{
    String filepath = Path::create(r_ctx->tempPath(), CACHE_FILE_NAME(0));

    for(size_t i = 0; Path::exists(filepath); i++)
        filepath = Path::create(r_ctx->tempPath(), CACHE_FILE_NAME(i));

    return filepath;
}

} // namespace REDasm
