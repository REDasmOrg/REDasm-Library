#include "instructioncache.h"
#include "../../mdbx/mdbxexception.h"
#include "../../mdbx/mdbx.h"
#include "../../support/utils.h"
#include "../../context.h"
#include <filesystem>

#define CACHE_FILE_NAME(x) ("redasm_cache_" + Utils::number(x) + ".tmp")

InstructionCache::InstructionCache(): m_filepath(generateFilePath()) { m_lmdb.open(m_filepath, MDBX_INTEGERKEY | MDBX_CREATE); }

InstructionCache::~InstructionCache()
{
    m_lockserialization = true;
    m_loaded.clear();

    m_lmdb.close();
    std::remove(m_filepath.c_str());
}

bool InstructionCache::contains(rd_address address) const { return m_cache.find(address) != m_cache.end(); }

bool InstructionCache::lock(rd_address address, RDInstruction** instruction) const
{
    if(!instruction || (m_cache.find(address) == m_cache.end()))
        return false;

    auto it = m_loaded.find(address);

    if(it == m_loaded.end())
    {
        RDInstruction ci{ };
        auto t = m_lmdb.transaction();

        try {
            t->get(address, &ci);
            t->commit();
        }
        catch(const MDBXException& e) {
            rd_ctx->problem(std::string(e.what()) + " @ " + Utils::hex(address));
            t->abort();
            return false;
        }

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
    if(it == m_loaded.end()) return false;

    size_t refcount = --it->second.refcount;
    if(refcount) return true;

    this->write(instruction);
    m_loaded.erase(it);
    return true;
}

void InstructionCache::erase(rd_address address) { m_cache.erase(address); }

bool InstructionCache::write(const RDInstruction* instruction) const
{
    if(m_lockserialization) return false;

    auto t = m_lmdb.transaction();
    t->putr(instruction->address, instruction);
    t->commit();
    return true;
}

void InstructionCache::cache(const RDInstruction* instruction) { if(this->write(instruction)) m_cache.insert(instruction->address); }

std::string InstructionCache::generateFilePath()
{
    auto filepath = std::filesystem::path(rd_ctx->tempPath()).append(CACHE_FILE_NAME(0));

    for(size_t i = 0; std::filesystem::exists(filepath); i++)
        filepath = std::filesystem::path(rd_ctx->tempPath()).append(CACHE_FILE_NAME(i));

    return filepath;
}
