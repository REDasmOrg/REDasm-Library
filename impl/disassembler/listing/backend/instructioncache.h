#pragma once

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <fstream>
#include <list>
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
        size_t size() const;
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
        std::map<address_t, std::streamoff> m_cache;
        std::fstream m_file;
        LMDB m_lmdb;
        String m_filepath;
        bool m_lockserialization;

    private:
        mutable std::recursive_mutex m_mutex;

    friend class CachedInstructionImpl;
};

} // namespace REDasm
