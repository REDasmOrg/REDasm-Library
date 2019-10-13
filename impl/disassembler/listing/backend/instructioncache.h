#pragma once

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <fstream>
#include <list>
#include <redasm/disassembler/listing/cachedinstruction.h>
#include <redasm/redasm.h>

namespace REDasm {

class InstructionCache
{
    public:
        InstructionCache();
        ~InstructionCache();
        size_t size() const;
        bool contains(address_t address) const;
        CachedInstruction find(address_t address);
        CachedInstruction findNearest(address_t address);
        CachedInstruction allocate(address_t address);
        CachedInstruction next(address_t address);
        CachedInstruction prev(address_t address);
        address_location nextHint(address_t address);
        address_location prevHint(address_t address);
        void erase(address_t address);

    private:
        void deallocate(const CachedInstruction &instruction);
        void serialize(const CachedInstruction &instruction);
        CachedInstruction deserialize(address_t address);
        static String generateFilePath();

    private:
        mutable std::unordered_map<address_t, CachedInstruction> m_loaded;
        std::map<address_t, std::streamoff> m_cache;
        std::fstream m_file;
        String m_filepath;
        bool m_lockserialization;

    private:
        mutable std::recursive_mutex m_mutex;

    friend class CachedInstructionImpl;
};

} // namespace REDasm
