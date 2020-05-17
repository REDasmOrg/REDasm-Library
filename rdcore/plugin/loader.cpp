#include "loader.h"
#include "../document/backend/segmentcontainer.h"
#include "../support/utils.h"
#include "../context.h"

Loader::Loader(const RDLoaderRequest* req, RDLoaderPlugin* ploader): m_ploader(ploader)
{
    m_document = SafeDocument(new Document());
    m_buffer.reset(reinterpret_cast<MemoryBuffer*>(req->buffer));
    m_filepath = req->filepath;
}

Loader::~Loader() { Context::freePlugin(reinterpret_cast<RDPluginHeader*>(&m_ploader)); }
void Loader::load() { if(m_ploader->load) m_ploader->load(m_ploader, CPTR(RDLoader, this)); }
void Loader::build(const RDLoaderBuildRequest* req) { if(m_ploader->build) m_ploader->build(m_ploader, req); }

bool Loader::analyze(Disassembler* disassembler)
{
    if(!m_ploader->analyze) return false;
    m_ploader->analyze(m_ploader, CPTR(RDDisassembler, disassembler));
    return true;
}

BufferView* Loader::view(address_t address) const { return this->view(address, RD_NPOS); }

BufferView* Loader::view(address_t address, size_t size) const
{
    RDLocation loc = this->offset(address);
    return loc.valid ? m_buffer->view(loc.value, size) : nullptr;
}

BufferView* Loader::view(const RDSegment& segment) const
{
    if(HAS_FLAG(&segment, SegmentFlags_Bss)) return nullptr;
    return m_buffer->view(segment.offset, SegmentContainer::offsetSize(segment));
}

flag_t Loader::flags() const { return m_ploader->flags; }
MemoryBuffer* Loader::buffer() { return m_buffer.get(); }
SafeDocument& Loader::document() { return m_document; }

RDLocation Loader::offset(address_t address) const
{
    RDSegment segment;
    if(!m_document->segment(address, &segment)|| HAS_FLAG(&segment, SegmentFlags_Bss)) return { {0}, false };

    address -= segment.address;
    address += segment.offset;
    return { {address}, true };
}

RDLocation Loader::address(offset_t offset) const
{
    RDSegment segment;
    if(!m_document->segmentOffset(offset, &segment)) return { {0}, false };

    offset -= segment.offset;
    offset += segment.address;
    return { {offset}, true };
}

RDLocation Loader::addressof(const void* ptr) const
{
    if(!m_buffer->contains(reinterpret_cast<const u8*>(ptr))) return { {0}, false };
    RDLocation loc = this->fileoffset(ptr);
    if(!loc.valid) return { {0}, false };
    return this->address(loc.value);
}

RDLocation Loader::fileoffset(const void* ptr) const
{
    if(!m_buffer->contains(reinterpret_cast<const u8*>(ptr))) return { {0}, false };
    return { {static_cast<location_t>(reinterpret_cast<const u8*>(ptr) - m_buffer->data())}, true };
}

u8* Loader::addrpointer(address_t address) const
{
  RDLocation loc = this->offset(address);
  return loc.valid ? Utils::relpointer(m_buffer->data(), loc.value) : nullptr;
}

u8* Loader::pointer(offset_t offset) const
{
    if(offset >= m_buffer->size()) return nullptr;
    return Utils::relpointer(m_buffer->data(), offset);
}
