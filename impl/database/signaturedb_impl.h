#pragma once

#include <redasm/database/signaturedb.h>
#include <redasm/pimpl.h>

namespace REDasm {

class SignatureDBImpl
{
    PIMPL_DECLARE_Q(SignatureDB)
    PIMPL_DECLARE_PUBLIC(SignatureDB)

    public:
        SignatureDBImpl();
        bool isCompatible() const;
        String assembler() const;
        String name() const;
        size_t size() const;
        const json& at(size_t index) const;
        void setAssembler(const String& assembler);
        void setName(const String& name);
        bool load(const String& sigfilename);
        bool save(const String& sigfilename);
        void search(const BufferView& view, const SignatureDB::SignatureFound& cb) const;
        bool searchSignature(const BufferView& view, const json &sig, const SignatureDB::SignatureFound& cb) const;
        bool checkPatterns(const BufferView& view, offset_t offset, const json &sig) const;

    private:
        json m_json;
};

} // namespace REDasm
