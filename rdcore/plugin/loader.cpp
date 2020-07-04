#include "loader.h"
#include "../support/utils.h"
#include "../context.h"

Loader::Loader(const RDLoaderRequest* req, RDLoaderPlugin* ploader): m_ploader(ploader)
{
    m_document = SafeDocument(new Document());
    m_buffer.reset(reinterpret_cast<MemoryBuffer*>(req->buffer));
    m_filepath = req->filepath;
}

Loader::~Loader() { Context::freePlugin(reinterpret_cast<RDPluginHeader*>(&m_ploader)); }

bool Loader::load()
{
    if(!m_ploader->load) return false;
    return m_ploader->load(m_ploader, CPTR(RDLoader, this));
}

bool Loader::build(const RDLoaderBuildRequest* req)
{
    if(!m_ploader->build) return false;
    return m_ploader->build(m_ploader, req);
}

bool Loader::analyze(Disassembler* disassembler)
{
    if(!m_ploader->analyze) return false;
    m_ploader->analyze(m_ploader, CPTR(RDDisassembler, disassembler));
    return true;
}

BufferView* Loader::view(rd_address address) const { return this->view(address, RD_NPOS); }

BufferView* Loader::view(rd_address address, size_t size) const
{
    RDLocation loc = this->offset(address);
    return loc.valid ? m_buffer->view(loc.value, size) : nullptr;
}

BufferView* Loader::view(const RDSegment& segment) const
{
    if(HAS_FLAG(&segment, SegmentFlags_Bss)) return nullptr;
    return m_buffer->view(segment.offset, SegmentContainer::offsetSize(segment));
}

Database* Loader::database(const std::string& dbname)
{
    auto it = m_database.find(dbname);
    if(it != m_database.end()) return it->second.get();

    auto* db = Database::load(dbname);
    if(!db) return nullptr;

    auto iit = m_database.emplace(dbname, db);
    return iit.first->second.get();
}

rd_flag Loader::flags() const { return m_ploader->flags; }
MemoryBuffer* Loader::buffer() { return m_buffer.get(); }
SafeDocument& Loader::document() { return m_document; }

RDLocation Loader::offset(rd_address address) const
{
    RDSegment segment;
    if(!m_document->segment(address, &segment)|| HAS_FLAG(&segment, SegmentFlags_Bss)) return { {0}, false };

    address -= segment.address;
    address += segment.offset;
    return { {address}, true };
}

RDLocation Loader::address(rd_offset offset) const
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
