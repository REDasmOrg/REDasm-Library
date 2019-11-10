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
    m_cache.clear();

    m_lmdb.close();
    std::remove(m_filepath.c_str());
}

size_t InstructionCache::size() const { std::lock_guard<std::recursive_mutex> lock(m_mutex); return m_cache.size(); }
bool InstructionCache::contains(address_t address) const { std::lock_guard<std::recursive_mutex> lock(m_mutex); return m_cache.find(address) != m_cache.end(); }

CachedInstruction InstructionCache::find(address_t address) { return this->deserialize(address); }

CachedInstruction InstructionCache::allocate()
{
    CachedInstruction ci(this);
    ci.pimpl_p()->m_instruction = std::make_shared<Instruction>();
    return ci;
}

void InstructionCache::store(const CachedInstruction& instruction) { m_loaded[instruction->address] = instruction.pimpl_p()->m_instruction; }
void InstructionCache::erase(address_t address) { m_cache.erase(address); }

void InstructionCache::serialize(const InstructionPtr& instruction)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
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
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    CachedInstruction cachedinstruction(this);
    auto it = m_loaded.find(address);

    if(it == m_loaded.end())
    {
        auto instruction = std::make_shared<Instruction>();

        auto t = m_lmdb.transaction();
        t->get(address, static_cast<InstructionStruct*>(instruction.get()));

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
