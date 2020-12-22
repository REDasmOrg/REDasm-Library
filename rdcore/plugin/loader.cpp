#include "loader.h"
#include "../document/document.h"
#include "../support/utils.h"

Loader::Loader(const std::shared_ptr<MemoryBuffer>& buffer, const std::string& filepath, const RDEntryLoader* entry, Context* ctx): Entry<RDEntryLoader>(entry, ctx), m_buffer(buffer), m_filepath(filepath)
{
    m_document = SafeDocument(new Document(ctx));
}

bool Loader::load()
{
    if(!m_entry->load) return false;
    return m_entry->load(CPTR(RDContext, this->context()), CPTR(RDLoader, this));
}

bool Loader::build()
{
    if(!m_entry->build) return false;
    return m_entry->build(CPTR(RDContext, this->context()), CPTR(RDLoader, this), &m_buildparams);
}

bool Loader::view(rd_address address, RDBufferView* view) const { return this->view(address, RD_NVAL, view); }

bool Loader::view(rd_address address, size_t size, RDBufferView* view) const
{
    RDLocation loc = this->offset(address);
    if(loc.valid) return m_buffer->view(loc.value, size, view);
    return { };
}

bool Loader::view(const RDSegment& segment, RDBufferView* view) const
{
    if(HAS_FLAG(&segment, SegmentFlags_Bss)) return { };
    return m_buffer->view(segment.offset, SegmentContainer::offsetSize(segment), view);
}

bool Loader::view(const RDBlock& block, RDBufferView* view) const
{
    size_t sz = BlockContainer::size(&block);
    return sz ? this->view(block.address, sz, view) : false;
}

rd_endianness Loader::endianness() const { return m_endianness; }
rd_flag Loader::flags() const { return m_entry->flags; }
MemoryBuffer* Loader::buffer() { return m_buffer.get(); }
SafeDocument& Loader::document() { return m_document; }

void Loader::setEndianness(rd_endianness endianness) { m_endianness = endianness; }
bool Loader::isAddress(rd_address address) const { return m_document->segments()->find(address, nullptr); }

RDLocation Loader::offset(rd_address address) const
{
    RDSegment segment;
    if(!m_document->segment(address, &segment)|| HAS_FLAG(&segment, SegmentFlags_Bss)) return { {0}, false };

    address -= segment.address;
    address += segment.offset;
    return { {address}, address < segment.endoffset };
}

RDLocation Loader::address(rd_offset offset) const
{
    RDSegment segment;
    if(!m_document->segmentOffset(offset, &segment)) return { {0}, false };

    offset -= segment.offset;
    offset += segment.address;
    return { {offset}, offset < segment.endaddress };
}

RDLocation Loader::addressof(const void* ptr) const
{
    if(!m_buffer->contains(reinterpret_cast<const u8*>(ptr))) return { {0}, false };
    RDLocation loc = this->fileoffset(ptr);
    if(!loc.valid) return { {0}, false };
    return this->address(loc.offset);
}

RDLocation Loader::fileoffset(const void* ptr) const
{
    if(!m_buffer->contains(reinterpret_cast<const u8*>(ptr))) return { {0}, false };
    return { {static_cast<rd_location>(reinterpret_cast<const u8*>(ptr) - m_buffer->data())}, true };
}

u8* Loader::addrpointer(rd_address address) const
{
  RDLocation loc = this->offset(address);
  return loc.valid ? Utils::relpointer(m_buffer->data(), loc.value) : nullptr;
}

u8* Loader::pointer(rd_offset offset) const
{
    if(offset >= m_buffer->size()) return nullptr;
    return Utils::relpointer(m_buffer->data(), offset);
}

const char* Loader::test(const RDEntryLoader* entry, const RDLoaderRequest* req)
{
    if(!entry->test) return nullptr;
    return entry->test(req);
}
