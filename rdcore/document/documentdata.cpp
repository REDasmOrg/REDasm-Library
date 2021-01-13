#include "documentdata.h"
#include "backend/segmentcontainer.h"
#include "backend/blockcontainer.h"
#include "../support/utils.h"
#include "../context.h"

DocumentData::DocumentData(const MemoryBufferPtr& buffer, Context* ctx): Listing(ctx), m_buffer(buffer) { }
rd_endianness DocumentData::endianness() const { return m_endianness; }
MemoryBuffer* DocumentData::buffer() const { return m_buffer.get(); }
void DocumentData::setEndianness(rd_endianness endianness) { m_endianness = endianness; }
bool DocumentData::view(rd_address address, RDBufferView* view) const { return this->view(address, RD_NVAL, view); }

bool DocumentData::view(rd_address address, size_t size, RDBufferView* view) const
{
    RDLocation loc = this->offset(address);
    if(loc.valid) return m_buffer->view(loc.value, size, view);
    return { };
}

bool DocumentData::view(const RDSegment& segment, RDBufferView* view) const
{
    if(HAS_FLAG(&segment, SegmentFlags_Bss)) return { };
    return m_buffer->view(segment.offset, SegmentContainer::offsetSize(segment), view);
}

bool DocumentData::view(const RDBlock& block, RDBufferView* view) const
{
    RDSegment segment;
    if(!this->segment(block.address, &segment) || HAS_FLAG(&segment, SegmentFlags_Bss)) return false;

    RDLocation loc = this->offset(block.address);
    if(!loc.valid) return false;

    // Limit BufferView's size to this segment, if needed
    auto bs = BlockContainer::size(&block), ss = segment.endoffset - loc.offset;
    return m_buffer->view(loc.offset, std::min(bs, ss), view);
}

RDLocation DocumentData::dereference(rd_address address) const
{
    RDLocation loc;

    u64 ptrvalue = 0;
    loc.valid = this->readAddress(address, &ptrvalue);
    if(loc.valid) loc.address = static_cast<rd_address>(ptrvalue);
    return loc;
}

bool DocumentData::readAddress(rd_address address, u64* value) const { return this->readAddress(address, this->context()->addressWidth(), value); }

bool DocumentData::readAddress(rd_address address, size_t size, u64* value) const
{
    RDBufferView view;
    if(!this->view(address, &view)) return false;

    switch(size)
    {
        case 1:  if(value) *value = *reinterpret_cast<u8*>(view.data);  break;
        case 2:  if(value) *value = *reinterpret_cast<u16*>(view.data); break;
        case 4:  if(value) *value = *reinterpret_cast<u32*>(view.data); break;
        case 8:  if(value) *value = *reinterpret_cast<u64*>(view.data); break;
        default: this->context()->problem("Invalid size: " + Utils::number(size)); return false;
    }

    return true;
}

const char* DocumentData::getHexDump(rd_address address, size_t size) const
{
    static std::string hexdump;

    RDBufferView view;
    if(!this->view(address, size, &view)) return nullptr;

    hexdump = Utils::hexString(&view);
    return hexdump.c_str();
}

const char16_t* DocumentData::readWString(rd_address address, size_t* len) const { return this->readStringT<char16_t>(address, len); }
const char* DocumentData::readString(rd_address address, size_t* len) const { return this->readStringT<char>(address, len); }
std::string DocumentData::readWString(rd_address address, size_t len) const { const char16_t* s = this->readWString(address, &len); return s ? Utils::toString(std::u16string(s, len)) : std::string(); }

std::string DocumentData::readString(rd_address address, size_t len) const
{
    const char* s = this->readString(address, &len);
    return s ? std::string(s, len) : std::string();
}

bool DocumentData::isAddress(rd_address address) const { return this->segments()->find(address, nullptr); }

RDLocation DocumentData::offset(rd_address address) const
{
    RDSegment segment;
    if(!this->segment(address, &segment)|| HAS_FLAG(&segment, SegmentFlags_Bss)) return { {0}, false };

    address -= segment.address;
    address += segment.offset;
    return { {address}, address < segment.endoffset };
}

RDLocation DocumentData::address(rd_offset offset) const
{
    RDSegment segment;
    if(!this->segmentOffset(offset, &segment)) return { {0}, false };

    offset -= segment.offset;
    offset += segment.address;
    return { {offset}, offset < segment.endaddress };
}

RDLocation DocumentData::addressof(const void* ptr) const
{
    if(!m_buffer->contains(reinterpret_cast<const u8*>(ptr))) return { {0}, false };
    RDLocation loc = this->fileoffset(ptr);
    if(!loc.valid) return { {0}, false };
    return this->address(loc.offset);
}

RDLocation DocumentData::fileoffset(const void* ptr) const
{
    if(!m_buffer->contains(reinterpret_cast<const u8*>(ptr))) return { {0}, false };
    return { {static_cast<rd_location>(reinterpret_cast<const u8*>(ptr) - m_buffer->data())}, true };
}

u8* DocumentData::addrpointer(rd_address address) const
{
  RDLocation loc = this->offset(address);
  return loc.valid ? Utils::relpointer(m_buffer->data(), loc.value) : nullptr;
}

u8* DocumentData::offspointer(rd_offset offset) const
{
    if(offset >= m_buffer->size()) return nullptr;
    return Utils::relpointer(m_buffer->data(), offset);
}
