#pragma once

#include <unordered_map>
#include <unordered_set>
#include <redasm/disassembler/listing/cachedinstruction.h>
#include <redasm/redasm.h>
#include "../lmdb/lmdb.h"

namespace REDasm {

class InstructionCache
{
    public:
        typedef std::shared_ptr<Instruction> InstructionPtr;

    public:
        InstructionCache();
        ~InstructionCache();
        bool contains(address_t address) const;
        CachedInstruction find(address_t address);
        CachedInstruction allocate();
        void store(const CachedInstruction& instruction);
        void serialize(const InstructionPtr& instruction);
        void erase(address_t address);

    private:
        CachedInstruction deserialize(address_t address);
        static String generateFilePath();

    private:
        mutable std::unordered_map<address_t, InstructionPtr> m_loaded;
        std::unordered_set<address_t> m_cached;
        LMDB m_lmdb;
        String m_filepath;
        bool m_lockserialization;

    private:
        using cache_lock = std::lock_guard<std::mutex>;
        mutable std::mutex m_mutex;

    friend class CachedInstructionImpl;
};

} // namespace REDasm
