#pragma once

#include <functional>
#include "../libs/json/json.hpp"
#include "../buffer/bufferview.h"
#include "../types/base.h"
#include "../pimpl.h"

#define SDB_VERSION          1
#define SIGNATURE_FIELD(T, name) T name() const { return (*this)[#name]; } \
                                 void name(const T& v) { (*this)[#name] = v; }

namespace REDasm {

using json = nlohmann::json;
class Disassembler;

struct SignaturePattern: public json {
    SignaturePattern();

    SIGNATURE_FIELD(offset_t, offset)
    SIGNATURE_FIELD(u32, size)
    SIGNATURE_FIELD(u16, checksum) // CRC-16
};

struct Signature: public json
{
    Signature();
    json& patterns();
    const json& patterns() const;
    void patterns(const json& p);

    SIGNATURE_FIELD(u32, symboltype)
    SIGNATURE_FIELD(u32, size)
    SIGNATURE_FIELD(String, name)
};

class SignatureDBImpl;

class SignatureDB
{
    PIMPL_DECLARE_P(SignatureDB)
    PIMPL_DECLARE_PRIVATE(SignatureDB)

    public:
        typedef std::function<void(const json&)> SignatureFound;

    public:
        SignatureDB();
        bool isCompatible() const;
        String assembler() const;
        String name() const;
        size_t size() const;
        const json& at(size_t index) const;
        void setAssembler(const String& assembler);
        void setName(const String& name);
        bool load(const String& sigfilename);
        bool save(const String& sigfilename);
        void search(const BufferView& view, const SignatureFound& cb) const;
        SignatureDB& operator <<(const Signature &signature);
};

} // namespace REDasm
