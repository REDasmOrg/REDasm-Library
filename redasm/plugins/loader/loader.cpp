#include "loader.h"
#include <impl/plugins/loader/loadrequest_impl.h>
#include <impl/plugins/loader/loader_impl.h>
#include "analyzer.h"

namespace REDasm {

LoadRequest::LoadRequest(const std::string &filepath, AbstractBuffer *buffer): m_pimpl_p(new LoadRequestImpl(filepath, buffer)) { }
std::string LoadRequest::filePath() const { PIMPL_P(const LoadRequest); return p->filePath(); }
AbstractBuffer *LoadRequest::buffer() const { PIMPL_P(const LoadRequest); return p->buffer(); }
const BufferView &LoadRequest::view() const { PIMPL_P(const LoadRequest); return p->view(); }

Loader::Loader(): Plugin(new LoaderImpl(this)) { }
LoaderFlags Loader::flags() const { return LoaderFlags::None; }
AbstractBuffer *Loader::buffer() const { PIMPL_P(const Loader); return p->buffer(); }
BufferView REDasm::Loader::viewOffset(offset_t offset) const { PIMPL_P(const Loader); return p->viewOffset(offset); }
const BufferView &Loader::view() const { PIMPL_P(const Loader); return p->view(); }
BufferView REDasm::Loader::view(address_t address) const { PIMPL_P(const Loader); return p->view(address); }
BufferView REDasm::Loader::viewSegment(const REDasm::Segment *segment) const { PIMPL_P(const Loader); return p->viewSegment(segment);  }
ListingDocument &REDasm::Loader::createDocument() { PIMPL_P(Loader); return p->createDocument(); }
const ListingDocument &REDasm::Loader::document() const { PIMPL_P(const Loader); return p->document(); }
ListingDocument &REDasm::Loader::document() { PIMPL_P(Loader); return p->document(); }
SignatureIdentifiers &Loader::signatures() { PIMPL_P(Loader); return p->signatures(); }
Loader *Loader::signature(const std::string &sig) { PIMPL_P(Loader); p->signature(sig); return this; }
Analyzer *Loader::analyzer(Disassembler *disassembler) { PIMPL_P(Loader); return p->analyzer(disassembler);  }
AssemblerRequest REDasm::Loader::assembler() const { PIMPL_P(const Loader); return p->assembler(); }
void Loader::init(const LoadRequest& request) { PIMPL_P(Loader); p->init(request); }
Analyzer *Loader::createAnalyzer(Disassembler *disassembler) const { return new Analyzer(disassembler); }
void Loader::build(const std::string& assembler, offset_t offset, address_t baseaddress, address_t entrypoint) { PIMPL_P(Loader); return p->build(assembler, offset, baseaddress, entrypoint); }
address_location Loader::address(offset_t offset) const { PIMPL_P(const Loader); return p->address(offset); }
address_location Loader::reladdress(address_t absaddress) const { return REDasm::make_location(absaddress); }
address_location Loader::absaddress(address_t reladdress) const { return REDasm::make_location(reladdress); }
offset_location Loader::offset(address_t address) const { PIMPL_P(const Loader); return p->offset(address); }

} // namespace REDasm
