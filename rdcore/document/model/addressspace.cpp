#include "addressspace.h"
#include "../../support/utils.h"
#include "../../context.h"
#include <algorithm>
#include <tuple>

bool operator ==(const RDSegment& s1, const RDSegment& s2) {
    return std::tie(s1.address, s1.endaddress, s1.offset, s1.endoffset, s1.flags) ==
           std::tie(s2.address, s2.endaddress, s2.offset, s2.endoffset, s2.flags);
}

AddressSpace::AddressSpace(Context* ctx): Object(ctx) { }

bool AddressSpace::markUnknown(rd_address address, size_t size)
{
    auto* blocks = this->findBlocks(address);
    if(!blocks) return false;

    blocks->unknownSize(address, size);
    return true;
}

bool AddressSpace::markExplored(rd_address address, size_t size)
{
    auto* blocks = this->findBlocks(address);
    if(!blocks) return false;

    blocks->exploredSize(address, size);
    return true;
}

bool AddressSpace::markCode(rd_address address, size_t size, u16 info)
{
    auto* blocks = this->findBlocks(address);
    if(!blocks) return false;

    blocks->codeSize(address, size, info);
    return true;
}

bool AddressSpace::markData(rd_address address, size_t size)
{
    auto* blocks = this->findBlocks(address);
    if(!blocks) return false;

    blocks->dataSize(address, size);
    return true;
}

bool AddressSpace::markString(rd_address address, size_t size)
{
    auto* blocks = this->findBlocks(address);
    if(!blocks) return false;

    blocks->stringSize(address, size);
    return true;
}

bool AddressSpace::markInfo(rd_address address, rd_type type, u16 info)
{
    auto* blocks = this->findBlocks(address);
    if(!blocks) return false;

    blocks->info(address, type, info);
    return true;
}

bool AddressSpace::insert(const RDSegment& segment)
{
    RDSegment s;

    if(this->addressToSegment(segment.address, &s))
    {
        rd_cfg->log("Segment '" + std::string(segment.name) + "' overlaps '" + s.name + "'");
        return false;
    }

    if(this->addressToSegment(segment.endaddress - 1, &s))
    {
        rd_cfg->log("Segment '" + std::string(segment.name) + "' overlaps '" + s.name + "'");
        return false;
    }

    m_segments.insert(segment.address, segment);
    auto [it, _] = m_blocks.insert({ segment.address, { } });
    it->second.unknownSize(segment.address, AddressSpace::addressSize(segment));

    if(!HAS_FLAG(&segment, SegmentFlags_Bss))
    {
        auto size = std::max(AddressSpace::offsetSize(segment), AddressSpace::addressSize(segment));
        auto [bit, __] = m_buffers.emplace(segment.address, size);
        bit->second.copyFrom(this->context()->buffer(), segment.offset, AddressSpace::offsetSize(segment));
    }
    else
        m_buffers.emplace(segment.address, AddressSpace::addressSize(segment));


    spdlog::info("Creating segment '{}' @ {:x} -> {:x}", segment.name, segment.address, segment.endaddress);
    return true;
}

RDLocation AddressSpace::offset(rd_address address) const
{
    RDSegment segment;
    if(!this->addressToSegment(address, &segment)|| HAS_FLAG(&segment, SegmentFlags_Bss)) return { {0}, false };

    address -= segment.address;
    address += segment.offset;
    return { {address}, address < segment.endoffset };
}

RDLocation AddressSpace::address(rd_offset offset) const
{
    RDSegment segment;
    if(!this->offsetToSegment(offset, &segment)) return { {0}, false };

    offset -= segment.offset;
    offset += segment.address;
    return { {offset}, offset < segment.endaddress };
}

RDLocation AddressSpace::addressof(const void* ptr) const
{
    RDSegment segment;
    if(!this->pointerToSegment(ptr, &segment)) return { {0}, false };

    auto it = m_buffers.find(segment.address);
    if(it == m_buffers.end()) return { {0}, false };

    auto loc = static_cast<rd_location>(reinterpret_cast<const u8*>(ptr) - it->second.data());
    return { {segment.address + loc}, true };
}

BlockContainer* AddressSpace::findBlocks(rd_address address)
{
    for(auto& [startaddress, blocks] : m_blocks)
    {
        if(blocks.contains(address)) return &blocks;
    }

    return nullptr;
}

bool AddressSpace::empty() const { return m_segments.empty(); }

const BlockContainer* AddressSpace::getBlocks(rd_address address) const
{
    RDSegment segment;
    if(!this->addressToSegment(address, &segment)) return nullptr;

    auto it = m_blocks.find(segment.address);
    return (it != m_blocks.end()) ? std::addressof(it->second) : nullptr;
}

const BlockContainer* AddressSpace::getBlocksAt(size_t index) const
{
    if(index >= m_segments.size()) return nullptr;

    rd_address address = m_segments[index];
    if(address == RD_NVAL) return nullptr;

    auto it = m_blocks.find(address);
    return (it != m_blocks.end()) ? std::addressof(it->second) : nullptr;
}

rd_address AddressSpace::firstAddress() const
{
    if(m_segments.empty()) return 0;
    auto* segment = m_segments.find(m_segments.front());
    return segment ? segment->address : 0;
}

rd_address AddressSpace::lastAddress() const
{
    if(m_segments.empty()) return 0;
    auto* segment = m_segments.find(m_segments.back());
    return segment ? segment->endaddress : 0;
}

size_t AddressSpace::size() const { return m_segments.size(); }
size_t AddressSpace::data(const rd_address** addresses) const { return m_segments.data(addresses); }
size_t AddressSpace::indexOfSegment(const RDSegment* segment) const { return segment ? m_segments.indexOf(segment->address) : RD_NVAL; }

size_t AddressSpace::indexOfSegment(rd_address address) const
{
    auto it = std::find_if(m_segments.begin(), m_segments.end(), [&](rd_address addr) {
        return AddressSpace::containsAddress(m_segments.find(addr), address);
    });

    return (it != m_segments.end()) ? std::distance(m_segments.begin(), it) : RD_NVAL;
}

bool AddressSpace::pointerToSegment(const void* ptr, RDSegment* segment) const
{
    auto it = std::find_if(m_buffers.begin(), m_buffers.end(), [&](const auto& item) {
        return item.second.contains(reinterpret_cast<const u8*>(ptr));
    });

    return (it != m_buffers.end()) ? this->addressToSegment(it->first, segment) : false;
}

bool AddressSpace::addressToSegment(rd_address address, RDSegment* segment) const
{
    auto it = std::find_if(m_segments.begin(), m_segments.end(), [&](const auto& item) {
        return AddressSpace::containsAddress(m_segments.find(item), address);
    });

    if(it == m_segments.end()) return false;
    if(segment) return m_segments.find(*it, segment);
    return true;
}

bool AddressSpace::offsetToSegment(rd_offset offset, RDSegment* segment) const
{
    auto it = std::find_if(m_segments.begin(), m_segments.end(), [&](const auto& item) {
        return AddressSpace::containsOffset(m_segments.find(item), offset);
    });

    if(it == m_segments.end()) return false;
    if(segment) return m_segments.find(*it, segment);
    return true;
}

bool AddressSpace::addressToBlock(rd_address address, RDBlock* block) const
{
    auto* blocks = this->findBlocks(address);
    if(!blocks) return false;
    return blocks->get(address, block);
}

bool AddressSpace::indexToSegment(size_t index, RDSegment* segment) const
{
    if(index >= m_segments.size()) return false;

    if(segment)
    {
        rd_address address = m_segments[index];
        return this->addressToSegment(address, segment);
    }

    return true;
}

bool AddressSpace::addressToView(rd_address address, size_t size, RDBufferView* view) const
{
    auto loc = this->offset(address);
    return loc.valid ? this->offsetToView(loc.offset, size, view) : false;
}

bool AddressSpace::offsetToView(rd_offset offset, size_t size, RDBufferView* view) const
{
    RDSegment segment;
    if(!this->offsetToSegment(offset, &segment)) return false;

    auto it = m_buffers.find(segment.address);
    if(it == m_buffers.end()) return false;
    return view ? it->second.view(offset - segment.offset, size, view) : true;
}

size_t AddressSpace::addressSize(const RDSegment& segment)
{
    return (segment.address > segment.endaddress) ? 0 : (segment.endaddress - segment.address);
}

size_t AddressSpace::offsetSize(const RDSegment& segment)
{
    if(HAS_FLAG(&segment, SegmentFlags_Bss)) return 0;
    return (segment.offset > segment.endoffset) ? 0 : (segment.endoffset - segment.offset);
}

bool AddressSpace::containsAddress(const RDSegment* segment, rd_address address)
{
    return segment && ((address >= segment->address) && (address < segment->endaddress));
}

bool AddressSpace::containsOffset(const RDSegment* segment, rd_offset offset)
{
    if(!segment || HAS_FLAG(segment, SegmentFlags_Bss)) return false;
    return (offset >= segment->offset) && (offset < segment->endoffset);
}

u8* AddressSpace::addrpointer(rd_address address) const
{
    RDSegment segment;
    if(!this->addressToSegment(address, &segment)) return nullptr;

    auto it = m_buffers.find(segment.address);
    if(it == m_buffers.end()) return nullptr;

    auto buffoffset = address - segment.address;
    if(buffoffset >= it->second.size()) return nullptr;

    return Utils::relpointer(const_cast<u8*>(it->second.data()), buffoffset);
}

u8* AddressSpace::offspointer(rd_offset offset) const
{
    RDSegment segment;
    if(!this->offsetToSegment(offset, &segment) || HAS_FLAG(&segment, SegmentFlags_Bss)) return nullptr;

    auto it = m_buffers.find(segment.address);
    if(it == m_buffers.end()) return nullptr;

    auto buffoffset = offset - segment.offset;
    if(buffoffset >= it->second.size()) return nullptr;

    return Utils::relpointer(const_cast<u8*>(it->second.data()), buffoffset);
}

const MemoryBuffer* AddressSpace::getBuffer(rd_address address) const
{
    auto it = m_buffers.find(address);
    return (it != m_buffers.end()) ? std::addressof(it->second) : nullptr;
}
