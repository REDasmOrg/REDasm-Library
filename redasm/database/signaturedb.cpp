#include "signaturedb.h"
#include <impl/database/signaturedb_impl.h>

namespace REDasm {

SignaturePattern::SignaturePattern(): json() { }
Signature::Signature(): json() { (*this)["patterns"] = json::array(); }
json &Signature::patterns() { return (*this)["patterns"]; }
const json &Signature::patterns() const { return (*this)["patterns"]; }
void Signature::patterns(const json &p) { (*this)["patterns"] = p; }
SignatureDB::SignatureDB(): m_pimpl_p(new SignatureDBImpl()) { }
bool SignatureDB::isCompatible(const Disassembler* disassembler) const { PIMPL_P(const SignatureDB); return p->isCompatible(disassembler); }
String SignatureDB::assembler() const { PIMPL_P(const SignatureDB); return p->assembler(); }
String SignatureDB::name() const { PIMPL_P(const SignatureDB); return p->name();  }
size_t SignatureDB::size() const { PIMPL_P(const SignatureDB); return p->size(); }
const json &SignatureDB::at(size_t index) const { PIMPL_P(const SignatureDB); return p->at(index);  }
void SignatureDB::setAssembler(const String &assembler) { PIMPL_P(SignatureDB); p->setAssembler(assembler); }
void SignatureDB::setName(const String &name) { PIMPL_P(SignatureDB); p->setName(name); }
bool SignatureDB::load(const String &sigfilename) { PIMPL_P(SignatureDB); return p->load(sigfilename); }
bool SignatureDB::save(const String &sigfilename) { PIMPL_P(SignatureDB); return p->save(sigfilename); }
void SignatureDB::search(const BufferView &view, const SignatureDB::SignatureFound &cb) const { PIMPL_P(const SignatureDB); p->search(view, cb); }
SignatureDB &SignatureDB::operator <<(const Signature &signature) { PIMPL_P(SignatureDB); p->m_json["signatures"].push_back(signature); return *this; }

} // namespace REDasm
