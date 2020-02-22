#pragma once

#include <redasm/types/string.h>
#include <redasm/database/signaturedb.h>
#include <atomic>

namespace REDasm {

class SignatureScanner
{
    public:
        SignatureScanner() = default;
        const String& signatureName() const;
        bool load(const String& signame);
        void scan(const address_t address);
        size_t count() const;

    private:
        String m_signame;
        SignatureDB m_sigdb;
        std::atomic<size_t> m_count;
};

} // namespace REDasm

