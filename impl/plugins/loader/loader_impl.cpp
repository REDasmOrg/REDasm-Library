#include "loader_impl.h"
#include <redasm/plugins/loader/analyzer.h>
#include <redasm/types/containers/list.h>
#include <redasm/macros.h>

namespace REDasm {

LoaderImpl::LoaderImpl(Loader *loader): m_pimpl_q(loader), m_buffer(nullptr) { }
AbstractBuffer *LoaderImpl::buffer() const { return m_buffer; }
BufferView LoaderImpl::viewOffset(offset_t offset) const { return m_buffer->view(offset); }
BufferView LoaderImpl::viewOffset(offset_t offset, size_t size) const { return m_buffer->view(offset, size); }
const BufferView &LoaderImpl::view() const { return m_view; }
BufferView LoaderImpl::view(address_t address) const { return this->view(address, REDasm::npos); }

BufferView LoaderImpl::view(address_t address, size_t size) const
{
    offset_location offset = this->offset(address);

    if(!offset.valid)
        return BufferView();

    return this->viewOffset(offset, size);
}

BufferView LoaderImpl::viewSegment(const Segment *segment) const
{
    if(!segment || segment->empty() || segment->is(SegmentType::Bss))
        return BufferView();

    return m_buffer->view(segment->offset, segment->rawSize());
}

ListingDocumentNew& LoaderImpl::createDocumentNew() { m_documentnew = ListingDocumentNew(new ListingDocumentTypeNew()); return m_documentnew; }
const ListingDocumentNew &LoaderImpl::documentNew() const { return m_documentnew; }
ListingDocumentNew &LoaderImpl::documentNew() { return m_documentnew; }
SignatureIdentifiers &LoaderImpl::signatures() { return m_signatures; }
void LoaderImpl::signature(const String &sig) { m_signatures.insert(sig); }

offset_location LoaderImpl::offset(address_t address) const
{
    for(size_t i = 0; i < m_documentnew->segments()->size(); i++)
    {
        const Segment* segment = m_documentnew->segments()->at(i);

        if(!segment->contains(address))
            continue;

        offset_t offset = (address - segment->address) + segment->offset;
        return REDasm::make_location(offset, segment->containsOffset(offset));
    }

    return REDasm::invalid_location<offset_t>();
}

address_location LoaderImpl::address(offset_t offset) const
{
    for(size_t i = 0; i < m_documentnew->segments()->size(); i++)
    {
        const Segment* segment = m_documentnew->segments()->at(i);

        if(!segment->containsOffset(offset))
            continue;

        address_t address = (offset - segment->offset) + segment->address;
        return REDasm::make_location(address, segment->contains(address));
    }

    return REDasm::invalid_location<address_t>();
}

void LoaderImpl::build(const String &assembler, offset_t offset, address_t baseaddress, address_t entrypoint) { throw std::runtime_error("Invalid call to Loader::build()"); }

Analyzer *LoaderImpl::analyzer()
{
    PIMPL_Q(Loader);
    m_analyzer = std::unique_ptr<Analyzer>(q->createAnalyzer());
    return m_analyzer.get();
}

void LoaderImpl::init(const LoadRequest &request)
{
    m_buffer = request.buffer();
    m_view = request.view();     // Full View
    this->createDocumentNew();
}

AssemblerRequest LoaderImpl::assembler() const { return nullptr; }

} // namespace REDasm
