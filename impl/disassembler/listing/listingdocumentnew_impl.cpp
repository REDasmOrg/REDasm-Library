#include "listingdocumentnew_impl.h"
#include <redasm/context.h>

namespace REDasm {

ListingDocumentTypeNewImpl::ListingDocumentTypeNewImpl(ListingDocumentTypeNew *q): m_pimpl_q(q) { }
const BlockContainer* ListingDocumentTypeNewImpl::blocks() const { return &m_blocks; }
const ListingItems* ListingDocumentTypeNewImpl::items() const { return &m_items; }
const ListingSegments *ListingDocumentTypeNewImpl::segments() const { return &m_segments; }
const ListingFunctions *ListingDocumentTypeNewImpl::functions() const { return &m_functions; }
const SymbolTable* ListingDocumentTypeNewImpl::symbols() const { return &m_symbols; }
const InstructionCache* ListingDocumentTypeNewImpl::instructions() const { return &m_instructions; }
const ListingCursor& ListingDocumentTypeNewImpl::cursor() const { return m_cursor; }
ListingCursor& ListingDocumentTypeNewImpl::cursor() { return m_cursor; }
const Symbol* ListingDocumentTypeNewImpl::entry() const { return m_entry; }
void ListingDocumentTypeNewImpl::symbol(address_t address, SymbolType type, SymbolFlags flags, tag_t tag) { this->symbol(address, SymbolTable::name(address, type, flags), type, flags, tag); }

void ListingDocumentTypeNewImpl::symbol(address_t address, const String& name, SymbolType type, SymbolFlags flags, tag_t tag)
{
    const Symbol* symbol = this->symbol(address);
    if(symbol && (symbol->type > type)) return;

    ListingItemType itemtype = ListingItemType::SymbolItem;

    switch(type)
    {
        case SymbolType::FunctionNew:
            itemtype = ListingItemType::FunctionItem;
            m_functions.insert(address);
            break;

        default: break;
    }

    if(symbol) this->remove(address, itemtype);
    if(m_symbols.create(address, name, type, flags, tag)) this->insert(address, itemtype);
}

const ListingItem& ListingDocumentTypeNewImpl::insert(address_t address, ListingItemType type, size_t index)
{
    switch(type)
    {
        case ListingItemType::FunctionItem: this->insert(address, ListingItemType::EmptyItem); break;
        default: break;
    }

    size_t idx = m_items.insert(address, type, index);
    this->notify(idx, ListingDocumentAction::Inserted);
    return m_items.at(idx);
}

void ListingDocumentTypeNewImpl::remove(const ListingItem& item) { size_t idx = m_items.indexOf(item); this->removeAt(idx); }

void ListingDocumentTypeNewImpl::notify(size_t idx, ListingDocumentAction action)
{
    if(idx >= m_items.size()) return;

    PIMPL_Q(ListingDocumentTypeNew);
    ListingDocumentChangedEventArgs e(m_items.at(idx), idx, action);
    q->changed(e);
}

void ListingDocumentTypeNewImpl::block(address_t address, size_t size, SymbolType type, SymbolFlags flags)
{
    if(!m_symbols.create(address, type, flags)) return;

    BlockItem* b = m_blocks.dataSize(address, size);
    this->remove(address, size, b);
    this->insert(address, ListingItemType::SymbolItem);
}

void ListingDocumentTypeNewImpl::block(const CachedInstruction& instruction)
{
    BlockItem* b = m_blocks.codeSize(instruction->address, instruction->size);
    this->remove(instruction->address, instruction->size, b);
    this->insert(instruction->address, ListingItemType::InstructionItem);
}

bool ListingDocumentTypeNewImpl::rename(address_t address, const String& name)
{
    if(!m_symbols.rename(address, name))
        return false;

    this->notify(m_items.symbolIndex(address));
    return true;
}

const Symbol* ListingDocumentTypeNewImpl::symbol(address_t address) const
{
    const BlockItem* item = m_blocks.find(address);
    if(!item) return m_symbols.get(address);
    return m_symbols.get(item->start);
}

const Symbol* ListingDocumentTypeNewImpl::symbol(const String& name) const { return m_symbols.get(name); }

void ListingDocumentTypeNewImpl::removeAt(size_t idx)
{
    this->notify(idx, ListingDocumentAction::Removed);

    const ListingItem& item = m_items.at(idx);

    switch(item.type_new)
    {
        case ListingItemType::SymbolItem: m_symbols.erase(item.address_new); break;
        case ListingItemType::InstructionItem: m_instructions.erase(item.address_new); break;
        case ListingItemType::SegmentItem: m_segments.erase(item.address_new); break;

        case ListingItemType::FunctionItem:
            m_functions.erase(item.address_new);
            this->remove(item.address_new, ListingItemType::EmptyItem);
            break;

        default: break;
    }

    m_items.erase(idx);
}

void ListingDocumentTypeNewImpl::remove(address_t address, ListingItemType type)
{
    size_t idx = m_items.indexOf(address, type);

    while(idx != REDasm::npos)
    {
        this->removeAt(idx);
        idx = m_items.indexOf(address, type);
    }
}

void ListingDocumentTypeNewImpl::remove(address_t address, size_t size, BlockItem* newblock)
{
    size_t idx = m_items.itemIndex(address);
    if(idx == REDasm::npos) return;

    if(newblock->typeIs(BlockItemType::Data)) this->removeCode(address, size, idx);
    else if(newblock->typeIs(BlockItemType::Code)) this->removeData(address, size, idx);
}

void ListingDocumentTypeNewImpl::removeData(address_t address, size_t size, size_t startidx)
{
    address_t endaddress = address + size;
    size_t idx = startidx;
    ListingItem item = m_items.at(idx);

    while(!m_items.empty() && (item.address_new < endaddress))
    {
        item = m_items.at(idx);

        if(item.is(ListingItemType::SymbolItem))
        {
            Symbol* symbol = m_symbols.get(item.address_new);

            if(symbol && symbol->isData())
            {
                this->removeAt(idx);
                continue;
            }
        }

        idx++;
    }
}

void ListingDocumentTypeNewImpl::removeCode(address_t address, size_t size, size_t startidx)
{
    address_t endaddress = address + size;
    size_t idx = startidx;
    ListingItem item = m_items.at(idx);

    while(!m_items.empty() && (item.address_new < endaddress))
    {
        item = m_items.at(idx);

        if(item.is(ListingItemType::SymbolItem))
        {
            Symbol* symbol = m_symbols.get(item.address_new);

            if(symbol && symbol->isCode())
            {
                this->removeAt(idx);
                continue;
            }
        }
        else if(item.is(ListingItemType::InstructionItem))
        {
            this->removeAt(idx);
            continue;
        }

        idx++;
    }
}

} // namespace REDasm
