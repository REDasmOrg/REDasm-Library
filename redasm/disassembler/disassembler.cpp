#include "disassembler.h"
#include <impl/disassembler/disassembler_impl.h>

namespace REDasm {

Disassembler::Disassembler(Assembler *assembler, Loader *loader): m_pimpl_p(new DisassemblerImpl(assembler, loader)) { }
Loader *Disassembler::loader() const { PIMPL_P(const Disassembler); return p->loader(); }
Assembler *Disassembler::assembler() const { PIMPL_P(const Disassembler); return p->assembler(); }
const safe_ptr<ListingDocumentType> &Disassembler::document() const { PIMPL_P(const Disassembler); return p->document(); }
safe_ptr<ListingDocumentType> &Disassembler::document() { PIMPL_P(Disassembler); return p->document(); }
std::deque<ListingItem *> Disassembler::getCalls(address_t address) { PIMPL_P(Disassembler); return p->getCalls(address); }
ReferenceTable *Disassembler::references() { PIMPL_P(Disassembler); return p->references(); }
Printer *Disassembler::createPrinter() { PIMPL_P(Disassembler); return p->createPrinter(); }
ReferenceVector Disassembler::getReferences(address_t address) const { PIMPL_P(const Disassembler); return p->getReferences(address); }
ReferenceSet Disassembler::getTargets(address_t address) const { PIMPL_P(const Disassembler); return p->getTargets(address); }
Symbol *Disassembler::dereferenceSymbol(const Symbol *symbol, u64 *value) { PIMPL_P(Disassembler); return p->dereferenceSymbol(symbol, value); }
InstructionPtr Disassembler::disassembleInstruction(address_t address) { PIMPL_P(Disassembler); return p->disassembleInstruction(address); }
address_location Disassembler::getTarget(address_t address) const { PIMPL_P(const Disassembler); return p->getTarget(address); }
size_t Disassembler::getTargetsCount(address_t address) const { PIMPL_P(const Disassembler); return p->getTargetsCount(address);  }
size_t Disassembler::getReferencesCount(address_t address) const { PIMPL_P(const Disassembler); return p->getReferencesCount(address); }
size_t Disassembler::checkAddressTable(const InstructionPtr &instruction, address_t address) { PIMPL_P(Disassembler); return p->checkAddressTable(instruction, address); }
size_t Disassembler::locationIsString(address_t address, bool *wide) const { PIMPL_P(const Disassembler); return p->locationIsString(address, wide); }
size_t Disassembler::state() const { PIMPL_P(const Disassembler); return p->state(); }
std::string Disassembler::readString(const Symbol *symbol, size_t len) const { PIMPL_P(const Disassembler); return p->readString(symbol, len); }
std::string Disassembler::readString(address_t address, size_t len) const { PIMPL_P(const Disassembler); return p->readString(address, len); }
std::string Disassembler::readWString(const Symbol *symbol, size_t len) const { PIMPL_P(const Disassembler); return p->readWString(symbol, len); }
std::string Disassembler::readWString(address_t address, size_t len) const { PIMPL_P(const Disassembler); return p->readWString(address, len); }
std::string Disassembler::getHexDump(address_t address, const Symbol **ressymbol) { PIMPL_P(Disassembler); return p->getHexDump(address, ressymbol); }
BufferView Disassembler::getFunctionBytes(address_t address) { PIMPL_P(Disassembler); return p->getFunctionBytes(address); }
bool Disassembler::checkString(address_t fromaddress, address_t address) { PIMPL_P(Disassembler); return p->checkString(fromaddress, address); }
bool Disassembler::readAddress(address_t address, size_t size, u64 *value) const { PIMPL_P(const Disassembler); return p->readAddress(address, size, value); }
bool Disassembler::readOffset(offset_t offset, size_t size, u64 *value) const { PIMPL_P(const Disassembler); return p->readOffset(offset, size, value); }
bool Disassembler::dereference(address_t address, u64 *value) const { PIMPL_P(const Disassembler); return p->dereference(address, value); }
bool Disassembler::loadSignature(const std::string &signame) { PIMPL_P(Disassembler); return p->loadSignature(signame); }
bool Disassembler::busy() const { PIMPL_P(const Disassembler); return p->busy(); }
void Disassembler::popTarget(address_t address, address_t pointedby) { PIMPL_P(Disassembler); p->popTarget(address, pointedby); }
void Disassembler::pushTarget(address_t address, address_t pointedby) { PIMPL_P(Disassembler); p->pushTarget(address, pointedby); }
void Disassembler::pushReference(address_t address, address_t refby) { PIMPL_P(Disassembler); p->pushReference(address, refby); }
void Disassembler::checkLocation(address_t fromaddress, address_t address) { PIMPL_P(Disassembler); p->checkLocation(fromaddress, address); }
void Disassembler::computeBasicBlocks() { PIMPL_P(Disassembler); p->computeBasicBlocks(); }
void Disassembler::disassemble(address_t address) { PIMPL_P(Disassembler); p->disassemble(address); }
void Disassembler::disassemble() { PIMPL_P(Disassembler); return p->disassemble(); }
void Disassembler::stop() { PIMPL_P(Disassembler); p->stop(); }
void Disassembler::pause() { PIMPL_P(Disassembler); p->pause(); }
void Disassembler::resume() { PIMPL_P(Disassembler); p->resume(); }

} // namespace REDasm
