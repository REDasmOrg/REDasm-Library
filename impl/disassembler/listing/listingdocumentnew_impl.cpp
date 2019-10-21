#include "listingdocumentnew_impl.h"
#include <redasm/context.h>

namespace REDasm {

ListingDocumentTypeNewImpl::ListingDocumentTypeNewImpl(ListingDocumentTypeNew *q): m_pimpl_q(q)
{
    m_blocks.inserted.connect(this, std::bind(&ListingDocumentTypeNewImpl::onBlockInserted, this, std::placeholders::_1));
    m_blocks.erased.connect(this, std::bind(&ListingDocumentTypeNewImpl::onBlockErased, this, std::placeholders::_1));
}

const BlockContainer* ListingDocumentTypeNewImpl::blocks() const { return &m_blocks; }
const ListingItems* ListingDocumentTypeNewImpl::items() const { return &m_items; }
const ListingSegments *ListingDocumentTypeNewImpl::segments() const { return &m_segments; }
const ListingFunctions *ListingDocumentTypeNewImpl::functions() const { return &m_functions; }
const SymbolTable* ListingDocumentTypeNewImpl::symbols() const { return &m_symbols; }
const InstructionCache* ListingDocumentTypeNewImpl::instructions() const { return &m_instructions; }
const ListingCursor& ListingDocumentTypeNewImpl::cursor() const { return m_cursor; }
ListingCursor& ListingDocumentTypeNewImpl::cursor() { return m_cursor; }
const Symbol* ListingDocumentTypeNewImpl::entry() const { return m_entry; }

ListingItem ListingDocumentTypeNewImpl::functionStart(address_t address) const
{
    const BlockItem* block = m_blocks.find(address);
    if(!block) return ListingItem();

    auto location = m_functions.functionFromAddress(address);
    if(!location.valid) return ListingItem();

    PIMPL_Q(const ListingDocumentTypeNew);
    return q->itemFunction(location);
}

void ListingDocumentTypeNewImpl::symbol(address_t address, SymbolType type, SymbolFlags flags, tag_t tag) { this->symbol(address, SymbolTable::name(address, type, flags), type, flags, tag); }

void ListingDocumentTypeNewImpl::symbol(address_t address, const String& name, SymbolType type, SymbolFlags flags, tag_t tag)
{
    if(!this->canOverrideAddress(address, type, flags)) return;
    this->createSymbol(address, name, type, flags, tag);

    this->insert(address, (type == SymbolType::FunctionNew) ? ListingItemType::FunctionItem :
                                                              ListingItemType::SymbolItem);
}

const ListingItem& ListingDocumentTypeNewImpl::insert(address_t address, ListingItemType type, size_t index)
{
    switch(type)
    {
        case ListingItemType::FunctionItem: m_functions.insert(address); this->insert(address, ListingItemType::EmptyItem); break;
        default: break;
    }

    size_t idx = m_items.insert(address, type, index);
    this->notify(idx, ListingDocumentAction::Inserted);
    return m_items.at(idx);
}

void ListingDocumentTypeNewImpl::notify(size_t idx, ListingDocumentAction action)
{
    if(idx >= m_items.size()) return;

    PIMPL_Q(ListingDocumentTypeNew);
    ListingDocumentChangedEventArgs e(m_items.at(idx), idx, action);
    q->changed(e);
}

void ListingDocumentTypeNewImpl::block(address_t address, size_t size, SymbolType type, SymbolFlags flags) { this->block(address, size, String(), type, flags); }

void ListingDocumentTypeNewImpl::block(address_t address, size_t size, const String& name, SymbolType type, SymbolFlags flags)
{
    if(!this->canOverrideAddress(address, type, flags)) return;

    m_blocks.dataSize(address, size);
    this->symbol(address, name, type, flags);
}

void ListingDocumentTypeNewImpl::block(const CachedInstruction& instruction) { m_blocks.codeSize(instruction->address, instruction->size); }
void ListingDocumentTypeNewImpl::unexplored(address_t address, size_t size) { m_blocks.unexploredSize(address, size); }

bool ListingDocumentTypeNewImpl::rename(address_t address, const String& name)
{
    if(!m_symbols.rename(address, name)) return false;

    this->notify(m_items.symbolIndex(address));
    return true;
}

const Symbol* ListingDocumentTypeNewImpl::symbol(address_t address) const
{
    const Symbol* symbol =  m_symbols.get(address);
    if(symbol) return symbol;

    const BlockItem* item = m_blocks.find(address);
    if(item) return m_symbols.get(item->start);
    return nullptr;
}

const Symbol* ListingDocumentTypeNewImpl::symbol(const String& name) const { return m_symbols.get(name); }

void ListingDocumentTypeNewImpl::removeAt(size_t idx)
{
    this->notify(idx, ListingDocumentAction::Removed);
    ListingItem item = m_items.at(idx);
    m_items.erase(idx);

    switch(item.type_new)
    {
        case ListingItemType::InstructionItem: m_instructions.erase(item.address_new); break;
        case ListingItemType::SegmentItem: m_segments.erase(item.address_new); break;

        case ListingItemType::SymbolItem:
            if(!m_functions.contains(item.address_new)) m_symbols.erase(item.address_new); // Don't delete functions
            break;

        case ListingItemType::FunctionItem:
            m_functions.erase(item.address_new);
            m_symbols.erase(item.address_new);
            this->remove(item.address_new, ListingItemType::EmptyItem);
            break;

        default: break;
    }
}

const FunctionGraph* ListingDocumentTypeNewImpl::graph(address_t address) const { return m_functions.graph(address); }
FunctionGraph* ListingDocumentTypeNewImpl::graph(address_t address) { return m_functions.graph(address);  }
void ListingDocumentTypeNewImpl::graph(address_t address, FunctionGraph* graph) { m_functions.graph(address, graph); }

void ListingDocumentTypeNewImpl::segmentCoverage(address_t address, size_t coverage)
{
    size_t idx = m_segments.indexOf(address);

    if(idx == REDasm::npos)
    {
        r_ctx->problem("Cannot find segment @ " + String::hex(address));
        return;
    }

    this->segmentCoverageAt(idx, coverage);
}

void ListingDocumentTypeNewImpl::segmentCoverageAt(size_t idx, size_t coverage)
{
    Segment* segment = m_segments.at(idx);

    if((coverage == REDasm::npos) || (segment->coveragebytes == REDasm::npos)) segment->coveragebytes = coverage;
    else segment->coveragebytes += coverage;
}

void ListingDocumentTypeNewImpl::invalidateGraphs() { m_functions.invalidateGraphs(); }

void ListingDocumentTypeNewImpl::remove(address_t address, ListingItemType type)
{
    size_t idx = m_items.indexOf(address, type);

    if(idx == REDasm::npos)
    {
        r_ctx->problem("Cannot delete @ " + String::hex(address) + ", type: " + ListingItem::displayType(type).quoted());
        return;
    }

    this->removeAt(idx);
}

void ListingDocumentTypeNewImpl::createSymbol(address_t address, const String& name, SymbolType type, SymbolFlags flags, tag_t tag)
{
    if(r_disasm->needsWeak()) flags |= SymbolFlags::Weak;

    if(name.empty()) m_symbols.create(address, type, flags, tag);
    else m_symbols.create(address, name, type, flags, tag);
}

bool ListingDocumentTypeNewImpl::canOverrideAddress(address_t address, SymbolType type, SymbolFlags flags) const
{
    if(!m_segments.find(address)) return false; // Ignore out of segment addresses
    if(r_disasm->needsWeak()) flags |= SymbolFlags::Weak;

    const BlockItem* bi = m_blocks.find(address);
    if((flags & SymbolFlags::Weak) && bi->typeIs(BlockItemType::Code)) return false;

    const Symbol* symbol = this->symbol(address);
    if(!symbol) return true;

    if(symbol->type > type) return false;

    if((symbol->type == type))
    {
        if(symbol->flags == flags) return false;
        if(!symbol->isWeak() && (flags & SymbolFlags::Weak)) return false;
    }

    return true;
}

void ListingDocumentTypeNewImpl::onBlockInserted(EventArgs* e)
{
    const BlockItem* bi = variant_object<const BlockItem>(e->arg());

    switch(bi->type)
    {
        case BlockItemType::Unexplored: this->insert(bi->start, ListingItemType::UnexploredItem); break;
        //case BlockItemType::Data: this->insert(bi->start, ListingItemType::SymbolItem); break;         // Don't add SymbolItem automatically
        case BlockItemType::Code: this->insert(bi->start, ListingItemType::InstructionItem); break;
        default: break;
    }
}

void ListingDocumentTypeNewImpl::onBlockErased(EventArgs* e)
{
    BlockItem* bi = variant_object<BlockItem>(e->arg());

    switch(bi->type)
    {
        case BlockItemType::Unexplored: this->remove(bi->start, ListingItemType::UnexploredItem); break;
        case BlockItemType::Data: this->remove(bi->start, ListingItemType::SymbolItem); break;
        case BlockItemType::Code: this->remove(bi->start, ListingItemType::InstructionItem); break;
    }
}

} // namespace REDasm
