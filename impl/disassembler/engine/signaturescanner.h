#pragma once

#include <redasm/types/string.h>
#include <redasm/database/signaturedb.h>
#include <atomic>

namespace REDasm {

class SignatureScanner
{
    public:
        SignatureScanner() = default;
        bool load(const String& signame);
        void scan(const address_t address);
        size_t count() const;

    private:
        SignatureDB m_sigdb;
        std::atomic<size_t> m_count;
};

} // namespace REDasm

