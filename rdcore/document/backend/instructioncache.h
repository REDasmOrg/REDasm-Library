#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <rdapi/types.h>
#include "../../mdbx/mdbx.h"

class InstructionCache
{
    public:
        struct CachedInstruction { RDInstruction instruction; size_t refcount; };

    public:
        InstructionCache();
        ~InstructionCache();

    public:
        bool lock(rd_address address, RDInstruction** instruction) const;
        bool unlock(const RDInstruction* instruction) const;

    public:
        bool contains(rd_address address) const;
        void cache(const RDInstruction* instruction);
        void erase(rd_address address);

    private:
        bool write(const RDInstruction* instruction) const;
        static std::string generateFilePath();

    private:
        mutable std::unordered_map<rd_address, CachedInstruction> m_loaded;
        std::unordered_set<rd_address> m_cache;
        std::string m_filepath;
        bool m_lockserialization{false};
        MDBX m_lmdb;
};
