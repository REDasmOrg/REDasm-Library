#pragma once

#include <unordered_map>
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
        CachedInstruction allocate(address_t address);

    private:
        void deallocate(const CachedInstruction &instruction);
        void serialize(const CachedInstruction &instruction);
        CachedInstruction deserialize(address_t address);
        static String generateFilePath();

    private:
        std::unordered_map<address_t, CachedInstruction> m_cache;
        std::unordered_map<address_t, std::streamoff> m_offsets;
        std::fstream m_file;
        String m_filepath;
        bool m_lockserialization;

    private:
        static std::unordered_set<String> m_activenames;
        mutable std::recursive_mutex m_mutex;

    friend class CachedInstructionImpl;
};

} // namespace REDasm
