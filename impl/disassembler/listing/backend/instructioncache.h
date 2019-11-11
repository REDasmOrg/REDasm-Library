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
        typedef std::weak_ptr<Instruction> InstructionWeakPtr;
        typedef std::shared_ptr<Instruction> InstructionPtr;

    public:
        InstructionCache();
        ~InstructionCache();
        bool contains(address_t address) const;
        CachedInstruction find(address_t address);
        CachedInstruction allocate();
        void cache(const CachedInstruction& instruction);
        void erase(address_t address);

    private:
        void allocate(CachedInstruction* ci);
        void serialize(Instruction* instruction);
        CachedInstruction deserialize(address_t address);
        static String generateFilePath();

    private:
        mutable std::unordered_map<address_t, InstructionWeakPtr> m_loaded;
        std::unordered_set<address_t> m_cached;
        LMDB m_lmdb;
        String m_filepath;
        bool m_lockserialization;

    friend class CachedInstructionImpl;
};

} // namespace REDasm
