#include "instructioncache.h"
#include <redasm/support/utils.h>
#include "../cachedinstruction_impl.h"
#include "../lmdb/lmdbexception.h"

#define CACHE_FILE_NAME(x)          ("redasm_cache_" + String::number(x) + ".tmp")

namespace REDasm {

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

bool InstructionCache::contains(address_t address) const { return m_cached.find(address) != m_cached.end(); }
CachedInstruction InstructionCache::find(address_t address) { return this->deserialize(address); }

CachedInstruction InstructionCache::allocate()
{
    CachedInstruction ci;
    this->allocate(&ci);
    return ci;
}

void InstructionCache::cache(const CachedInstruction& instruction) { m_cached.insert(instruction->address); }
void InstructionCache::erase(address_t address) { m_cached.erase(address); }

void InstructionCache::allocate(CachedInstruction* ci)
{
    ci->pimpl_p()->m_cache = this;
    ci->pimpl_p()->m_instruction = std::shared_ptr<Instruction>(new Instruction(), [&](Instruction* p) {
                                           this->serialize(p);
                                           std::default_delete<Instruction>()(p);
                                        });
}

void InstructionCache::serialize(Instruction* instruction)
{
    if(m_lockserialization) return;

    auto t = m_lmdb.transaction();
    t->putr(instruction->address, static_cast<const InstructionStruct*>(instruction));
    t->commit();
}

CachedInstruction InstructionCache::deserialize(address_t address)
{
    CachedInstruction ci;
    auto sp = m_loaded[address].lock();

    if(!sp)
    {
        this->allocate(&ci);
        auto t = m_lmdb.transaction();

        try {
            t->get(address, static_cast<InstructionStruct*>(ci.get()));
            t->commit();
        }
        catch(const LMDBException& e) {
            r_ctx->problem(String(e.what()) + " @ " + String::hex(address));
            t->abort();
            return ci; // Return an invalid instruction
        }

        // Reset backend specific fields
        ci->free = nullptr;
        ci->puserdata = nullptr;

        // weak_ptr<T> <- shared_ptr<T>
        m_loaded[address] = sp = ci.pimpl_p()->m_instruction;
    }
    else
    {
        ci.pimpl_p()->m_cache = this;
        ci.pimpl_p()->m_instruction = sp;
    }

    return ci;
}

String InstructionCache::generateFilePath()
{
    String filepath = Path::create(r_ctx->tempPath(), CACHE_FILE_NAME(0));

    for(size_t i = 0; Path::exists(filepath); i++)
        filepath = Path::create(r_ctx->tempPath(), CACHE_FILE_NAME(i));

    return filepath;
}

} // namespace REDasm
