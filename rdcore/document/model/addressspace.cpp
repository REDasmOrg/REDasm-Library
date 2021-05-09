#include "addressspace.h"
#include "../../support/demangler.h"
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

bool AddressSpace::markCode(rd_address address, size_t size)
{
    auto* blocks = this->findBlocks(address);
    if(!blocks) return false;

    blocks->codeSize(address, size);
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
        auto [bit, __] = m_buffers.emplace(segment.address, AddressSpace::offsetSize(segment));
        bit->second.copyFrom(this->context()->buffer(), segment.offset, AddressSpace::offsetSize(segment));
    }
    else
        m_buffers.emplace(segment.address, AddressSpace::addressSize(segment));

    return true;
}

void AddressSpace::setLabel(rd_address address, const std::string& label, rd_flag flags)
{
    auto info = this->getInfo(address);
    if(!info->weak && this->context()->needsWeak()) return;

    info->weak = this->context()->needsWeak();
    info->label = Demangler::demangled(label);
    info->flags |= flags;

    m_labels[info->label] = address;
    if(!flags) return;

    for(rd_flag f = 1; f < AddressFlags_Length; f <<= 1)
    {
        if(!(flags & f)) continue;

        if((f & AddressFlags_AsciiString) || (f & AddressFlags_WideString))
           m_labelflags[AddressFlags_String].insert(address);

        m_labelflags[f].insert(address);
    }
}

void AddressSpace::updateFlags(rd_address address, rd_flag flags, bool set)
{
    auto* info = m_info.find(address);
    if(!info) return;

    if(set) info->flags |= flags;
    else info->flags &= ~flags;
}

bool AddressSpace::updateLabel(rd_address address, const std::string& label)
{
    if(label.empty()) return false;

    auto* info = m_info.find(address);
    if(!info) return false;

    auto lit = m_labels.find(label);
    if(lit != m_labels.end()) return lit->second == address;

    info->label = label;
    m_labels.erase(info->label);
    m_labels[label] = address;
    return true;
}

void AddressSpace::addComment(rd_address address, const std::string& s)
{
    auto* info = this->getInfo(address);
    info->comments.push_back(s);
}

void AddressSpace::setComments(rd_address address, const AddressSpace::Comments& c)
{
    auto* info = this->getInfo(address);
    info->comments = c;
}

bool AddressSpace::setTypeField(rd_address address, const Type* type, int indent, const std::string& name)
{
    if(!type || !this->markData(address, type->size())) return false;

    auto* info = this->getInfo(address);
    info->typefield.reset(type->clone(this->context()));
    info->label = name;
    info->indent = indent;
    info->flags |= AddressFlags_TypeField;

    return true;
}

void AddressSpace::setType(rd_address address, const Type* type, const std::string& label)
{
    if(!type) return;

    auto* info = this->getInfo(address);
    info->type.reset(type->clone(this->context()));
    if(!label.empty()) info->type->setName(label);
    info->label = info->type->name();
    info->flags |= AddressFlags_Type;

    m_labels[info->label] = address;
}

const Type* AddressSpace::getTypeField(rd_address address, int* indent) const
{
    auto* info = m_info.find(address);
    if(!info) return nullptr;
    if(indent) *indent = info->indent;
    return info->typefield.get();
}

const Type* AddressSpace::getType(rd_address address) const
{
    auto* info = m_info.find(address);
    return info ? info->type.get() : nullptr;
}

rd_flag AddressSpace::getFlags(rd_address address) const
{
    auto* info = m_info.find(address);
    return info ? info->flags : AddressFlags_None;
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

bool AddressSpace::findLabel(const std::string& q, rd_address* resaddress) const
{
    return this->findLabelR(Utils::wildcardToRegex(q), resaddress);
}

bool AddressSpace::findLabelR(const std::string& q, rd_address* resaddress) const
{
    for(const auto& [label, addr] : m_labels)
    {
        if(!Utils::matchRegex(label, q)) continue;
        if(resaddress) *resaddress = addr;
        return true;
    }

    return false;
}

size_t AddressSpace::findLabels(const std::string& q, const rd_address** resaddresses) const { return this->findLabelsR(Utils::wildcardToRegex(q), resaddresses); }

size_t AddressSpace::findLabelsR(const std::string& q, const rd_address** resaddresses) const
{
    for(const auto& [label, addr] : m_labels)
    {
        if(!Utils::matchRegex(label, q)) continue;
        m_result.push_back(addr);
    }

    if(resaddresses) *resaddresses = m_result.data();
    return m_result.size();
}

const BlockContainer* AddressSpace::getBlocks(rd_address address) const
{
    RDSegment segment;
    if(!this->addressToSegment(address, &segment)) return nullptr;

    auto it = m_blocks.find(segment.address);
    return (it != m_blocks.end()) ? std::addressof(it->second) : nullptr;
}

const BlockContainer* AddressSpace::getBlocksAt(size_t index) const
{
    if(index >= m_blocks.size()) return nullptr;

    rd_address address = m_segments[index];
    if(address == RD_NVAL) return nullptr;

    auto it = m_blocks.find(address);
    return (it != m_blocks.end()) ? std::addressof(it->second) : nullptr;
}

std::optional<std::string> AddressSpace::getLabel(rd_address address) const
{
    auto* info = m_info.find(address);
    return info ? std::make_optional(info->label) : std::nullopt;
}

AddressSpace::Comments AddressSpace::getComments(rd_address address) const
{
    auto* info = m_info.find(address);
    return info ? info->comments : Comments{ };
}

rd_address AddressSpace::getAddress(const std::string& label) const
{
    auto it = m_labels.find(Demangler::demangled(label));
    return (it != m_labels.end()) ? it->second : RD_NVAL;
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
size_t AddressSpace::getLabels(const rd_address** addresses) const { return m_info.data(addresses); }

size_t AddressSpace::getLabelsByFlag(rd_flag flag, const rd_address** addresses) const
{
    auto it = m_labelflags.find(flag);
    return (it != m_labelflags.end()) ? it->second.data(addresses) : 0;
}

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

AddressSpace::Info* AddressSpace::getInfo(rd_address address)
{
    auto info = m_info.find(address);
    if(info) return info;

    m_info.insert(address, Info{ });
    return m_info.find(address);
}

u8* AddressSpace::addrpointer(rd_address address) const
{
    RDSegment segment;
    if(!this->addressToSegment(address, &segment)) return nullptr;

    auto it = m_buffers.find(segment.address);
    return (it != m_buffers.end()) ? Utils::relpointer(const_cast<u8*>(it->second.data()), address - segment.address) : nullptr;
}

u8* AddressSpace::offspointer(rd_offset offset) const
{
    RDSegment segment;
    if(!this->offsetToSegment(offset, &segment) || HAS_FLAG(&segment, SegmentFlags_Bss)) return nullptr;

    auto it = m_buffers.find(segment.address);
    return (it != m_buffers.end()) ? Utils::relpointer(const_cast<u8*>(it->second.data()), offset - segment.offset) : nullptr;
}

const MemoryBuffer* AddressSpace::getBuffer(rd_address address) const
{
    auto it = m_buffers.find(address);
    return (it != m_buffers.end()) ? std::addressof(it->second) : nullptr;
}
