#include "listingsegments.h"
#include <impl/disassembler/listing/backend/listingsegments_impl.h>
#include "../../../context.h"

namespace REDasm {

ListingSegments::ListingSegments(): m_pimpl_p(new ListingSegmentsImpl()) { }
bool ListingSegments::empty() const { PIMPL_P(const ListingSegments); return p->empty(); }
size_t ListingSegments::size() const { PIMPL_P(const ListingSegments); return p->size(); }

size_t ListingSegments::indexOf(address_t address) const
{
    for(size_t i = 0; i < this->size(); i++)
    {
        const Segment* segment = this->at(i);
        if(segment->contains(address)) return i;
    }

    return REDasm::npos;
}

Segment* ListingSegments::at(size_t idx) { return const_cast<Segment*>(static_cast<const ListingSegments*>(this)->at(idx)); }
Segment* ListingSegments::find(const String& name) { return const_cast<Segment*>(static_cast<const ListingSegments*>(this)->find(name)); }
Segment* ListingSegments::find(address_t address) { return const_cast<Segment*>(static_cast<const ListingSegments*>(this)->find(address)); }
const Segment *ListingSegments::at(size_t idx) const { PIMPL_P(const ListingSegments); return &(p->at(idx)); }

const Segment *ListingSegments::find(const String &name) const
{
    for(size_t i = 0; i < this->size(); i++)
    {
        const Segment* segment = this->at(i);

        if(segment->name == name)
            return segment;
    }

    return nullptr;
}

const Segment *ListingSegments::find(address_t address) const { size_t idx = this->indexOf(address); return idx != REDasm::npos ? this->at(idx) : nullptr; }
bool ListingSegments::insert(const String &name, offset_t offset, address_t address, u64 size, SegmentType type) { return this->insert(name, offset, address, size, size, type); }

bool ListingSegments::insert(const String &name, offset_t offset, address_t address, u64 psize, u64 vsize, SegmentType type)
{
    if(!psize && !vsize)
    {
        r_ctx->log("Skipping empty segment " + name.quoted());
        return false;
    }

    for(size_t i = 0; i < this->size(); i++)
    {
        const Segment* segment = this->at(i);

        if(segment->is(SegmentType::Bss) ? segment->contains(address) : ((segment->offset == offset) || segment->contains(address)))
        {
            r_ctx->problem("Segment " + name.quoted() + " overlaps " + segment->name.quoted());
            return false;
        }
    }

    PIMPL_P(ListingSegments);
    p->insert(Segment(name, offset, address, psize, vsize, type));
    return true;
}

void ListingSegments::erase(const Segment* segment) { PIMPL_P(ListingSegments); p->erase(*segment); }
void ListingSegments::erase(size_t idx) { PIMPL_P(ListingSegments); p->eraseAt(idx); }

} // namespace REDasm
