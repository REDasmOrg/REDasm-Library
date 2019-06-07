#pragma once

#include <redasm/database/signaturedb.h>
#include <redasm/pimpl.h>

namespace REDasm {

class SignatureDBImpl
{
    PIMPL_DECLARE_PUBLIC(SignatureDB)

    public:
        SignatureDBImpl();
        bool isCompatible(const Disassembler *disassembler) const;
        std::string assembler() const;
        std::string name() const;
        size_t size() const;
        const json& at(size_t index) const;
        void setAssembler(const std::string& assembler);
        void setName(const std::string& name);
        bool load(const std::string& sigfilename);
        bool save(const std::string& sigfilename);
        void search(const BufferView& view, const SignatureDB::SignatureFound& cb) const;
        void searchSignature(const BufferView& view, const json &sig, const SignatureDB::SignatureFound& cb) const;
        bool checkPatterns(const BufferView& view, offset_t offset, const json &sig) const;

    private:
        json m_json;
};

} // namespace REDasm
