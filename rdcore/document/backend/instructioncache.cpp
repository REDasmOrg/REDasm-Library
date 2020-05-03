#include "instructioncache.h"
#include "../../lmdb/lmdbexception.h"
#include "../../lmdb/lmdb.h"
#include "../../support/utils.h"
#include "../../context.h"
#include <filesystem>
#include <cassert>

#define CACHE_FILE_NAME(x) ("redasm_cache_" + Utils::number(x) + ".tmp")

InstructionCache::InstructionCache(): m_filepath(generateFilePath()), m_lockserialization(false)
{
    m_lmdb.open(m_filepath, MDB_INTEGERKEY | MDB_CREATE);
}

InstructionCache::~InstructionCache()
{
    m_lockserialization = true;
    m_loaded.clear();

    m_lmdb.close();
    std::remove(m_filepath.c_str());
}

bool InstructionCache::contains(address_t address) const { return m_cache.find(address) != m_cache.end(); }

bool InstructionCache::lock(address_t address, RDInstruction** instruction) const
{
    if(!instruction || (m_cache.find(address) == m_cache.end()))
        return false;

    RDInstruction ci{ };
    auto it = m_loaded.find(address);

    if(it == m_loaded.end())
    {
        auto t = m_lmdb.transaction();

        try {
            t->get(address, &ci);
            t->commit();
        }
        catch(const LMDBException& e) {
            rd_ctx->problem(std::string(e.what()) + " @ " + Utils::hex(address));
            t->abort();
            return false;
        }

        // Reset backend specific fields
        ci.free = nullptr;
        ci.userdata = 0;

        m_loaded[address] = { ci, 1 };
    }
    else
        it->second.refcount++;

    *instruction = std::addressof(m_loaded[address].instruction);
    return true;
}

bool InstructionCache::unlock(const RDInstruction* instruction) const
{
    if(!instruction) return false;

    auto it = m_loaded.find(instruction->address);
    assert(it != m_loaded.end());

    size_t refcount = --it->second.refcount;
    if(refcount) return true;

    m_loaded.erase(it);
    return true;
}

void InstructionCache::erase(address_t address) { m_cache.erase(address); }

void InstructionCache::cache(const RDInstruction* instruction)
{
    if(m_lockserialization) return;

    m_cache.insert(instruction->address);
    auto t = m_lmdb.transaction();
    t->putr(instruction->address, instruction);
    t->commit();
}

std::string InstructionCache::generateFilePath()
{
    auto filepath = std::filesystem::path(rd_ctx->tempPath()).append(CACHE_FILE_NAME(0));

    for(size_t i = 0; std::filesystem::exists(filepath); i++)
        filepath = std::filesystem::path(rd_ctx->tempPath()).append(CACHE_FILE_NAME(i));

    return filepath;
}
