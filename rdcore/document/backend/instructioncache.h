#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <rdapi/types.h>
#include "../../lmdb/lmdb.h"

class InstructionCache
{
    public:
        struct CachedInstruction { RDInstruction instruction; size_t refcount; };

    public:
        InstructionCache();
        ~InstructionCache();

    public:
        bool lock(address_t address, RDInstruction** instruction) const;
        bool unlock(const RDInstruction* instruction) const;

    public:
        bool contains(address_t address) const;
        void cache(const RDInstruction* instruction);
        void erase(address_t address);

    private:
        bool write(const RDInstruction* instruction) const;
        static std::string generateFilePath();

    private:
        mutable std::unordered_map<address_t, CachedInstruction> m_loaded;
        std::unordered_set<address_t> m_cache;
        std::string m_filepath;
        bool m_lockserialization{false};
        LMDB m_lmdb;
};
