#include "disassembler.h"
#include <impl/disassembler/disassembler_impl.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/context.h>

namespace REDasm {

Disassembler::Disassembler(Assembler *assembler, Loader *loader): m_pimpl_p(new DisassemblerImpl(this, assembler, loader)) { }
Disassembler::~Disassembler() { if(r_ctx->disassembler() == this) r_ctx->setDisassembler(nullptr); }
Loader *Disassembler::loader() const { PIMPL_P(const Disassembler); return p->loader(); }
Assembler *Disassembler::assembler() const { PIMPL_P(const Disassembler); return p->assembler(); }
const safe_ptr<ListingDocumentType> &Disassembler::document() const { PIMPL_P(const Disassembler); return p->document(); }
safe_ptr<ListingDocumentType> &Disassembler::document() { PIMPL_P(Disassembler); return p->document(); }
const safe_ptr<ListingDocumentTypeNew>& Disassembler::documentNew() const { PIMPL_P(const Disassembler); return p->documentNew(); }
safe_ptr<ListingDocumentTypeNew>& Disassembler::documentNew() { PIMPL_P(Disassembler); return p->documentNew(); }
ReferenceTable *Disassembler::references() { PIMPL_P(Disassembler); return p->references(); }
SortedSet Disassembler::getReferences(address_t address) const { PIMPL_P(const Disassembler); return p->getReferences(address); }
SortedSet Disassembler::getTargets(address_t address) const { PIMPL_P(const Disassembler); return p->getTargets(address); }
const Symbol *Disassembler::dereferenceSymbol(const Symbol *symbol, u64 *value) { PIMPL_P(Disassembler); return p->dereferenceSymbol(symbol, value); }
CachedInstruction Disassembler::disassembleInstruction(address_t address) { PIMPL_P(Disassembler); return p->disassembleInstruction(address); }
address_location Disassembler::getTarget(address_t address) const { PIMPL_P(const Disassembler); return p->getTarget(address); }
size_t Disassembler::getTargetsCount(address_t address) const { PIMPL_P(const Disassembler); return p->getTargetsCount(address);  }
size_t Disassembler::getReferencesCount(address_t address) const { PIMPL_P(const Disassembler); return p->getReferencesCount(address); }
size_t Disassembler::checkAddressTable(const CachedInstruction &instruction, address_t address) { PIMPL_P(Disassembler); return p->checkAddressTable(instruction, address); }
String Disassembler::readString(const Symbol *symbol, size_t len) const { PIMPL_P(const Disassembler); return p->readString(symbol, len); }
String Disassembler::readString(address_t address, size_t len) const { PIMPL_P(const Disassembler); return p->readString(address, len); }
String Disassembler::readWString(const Symbol *symbol, size_t len) const { PIMPL_P(const Disassembler); return p->readWString(symbol, len); }
String Disassembler::readWString(address_t address, size_t len) const { PIMPL_P(const Disassembler); return p->readWString(address, len); }
String Disassembler::getHexDump(address_t address, const Symbol **ressymbol) { PIMPL_P(Disassembler); return p->getHexDump(address, ressymbol); }
BufferView Disassembler::getFunctionBytes(address_t address) { PIMPL_P(Disassembler); return p->getFunctionBytes(address); }
bool Disassembler::readAddress(address_t address, size_t size, u64 *value) const { PIMPL_P(const Disassembler); return p->readAddress(address, size, value); }
bool Disassembler::readOffset(offset_t offset, size_t size, u64 *value) const { PIMPL_P(const Disassembler); return p->readOffset(offset, size, value); }
bool Disassembler::dereference(address_t address, u64 *value) const { PIMPL_P(const Disassembler); return p->dereference(address, value); }
bool Disassembler::loadSignature(const String &signame) { PIMPL_P(Disassembler); return p->loadSignature(signame); }
bool Disassembler::busy() const { PIMPL_P(const Disassembler); return p->busy(); }
bool Disassembler::needsWeak() const { PIMPL_P(const Disassembler); return p->needsWeak(); }
void Disassembler::popTarget(address_t address, address_t pointedby) { PIMPL_P(Disassembler); p->popTarget(address, pointedby); }
void Disassembler::pushTarget(address_t address, address_t pointedby) { PIMPL_P(Disassembler); p->pushTarget(address, pointedby); }
void Disassembler::pushReference(address_t address, address_t refby) { PIMPL_P(Disassembler); p->pushReference(address, refby); }
void Disassembler::checkLocation(address_t fromaddress, address_t address) { PIMPL_P(Disassembler); p->checkLocation(fromaddress, address); }
void Disassembler::disassemble(address_t address) { PIMPL_P(Disassembler); p->disassemble(address); }
void Disassembler::disassemble() { PIMPL_P(Disassembler); return p->disassemble(); }
void Disassembler::stop() { PIMPL_P(Disassembler); p->stop(); }

} // namespace REDasm
