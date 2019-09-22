#include "listingdocumentnew.h"
#include <impl/disassembler/listing/listingdocumentnew_impl.h>
#include <redasm/context.h>

namespace REDasm {

ListingDocumentTypeNew::ListingDocumentTypeNew(): m_pimpl_p(new ListingDocumentTypeNewImpl(this)) { }
const BlockContainer *ListingDocumentTypeNew::blocks() const { PIMPL_P(const ListingDocumentTypeNew); return p->blocks(); }
const ListingItems *ListingDocumentTypeNew::items() const { PIMPL_P(const ListingDocumentTypeNew); return p->items(); }
const ListingSegments *ListingDocumentTypeNew::segments() const { PIMPL_P(const ListingDocumentTypeNew); return p->segments(); }
const ListingFunctions *ListingDocumentTypeNew::functions() const { PIMPL_P(const ListingDocumentTypeNew); return p->functions(); }
const SymbolTable* ListingDocumentTypeNew::symbols() const { PIMPL_P(const ListingDocumentTypeNew); return p->symbols(); }
const ListingCursor& ListingDocumentTypeNew::cursor() const { PIMPL_P(const ListingDocumentTypeNew); return p->cursor(); }
ListingCursor& ListingDocumentTypeNew::cursor() { PIMPL_P(ListingDocumentTypeNew); return p->cursor(); }
const Symbol* ListingDocumentTypeNew::entry() const { PIMPL_P(const ListingDocumentTypeNew); return p->entry(); }

void ListingDocumentTypeNew::entry(address_t address)
{
    PIMPL_P(ListingDocumentTypeNew);

    String name;
    Symbol* symbol = p->m_symbols.symbol(address); // Don't override custom symbols, if any

    if(symbol)
        name = symbol->name;

    p->function(address, name.empty() ? ENTRY_FUNCTION : name, SymbolType::EntryPoint);
    this->setEntry(address);
}

void ListingDocumentTypeNew::empty(address_t address) { PIMPL_P(ListingDocumentTypeNew); p->insert(address, ListingItemType::EmptyItem); }

bool ListingDocumentTypeNew::separator(address_t address)
{
    PIMPL_P(ListingDocumentTypeNew);

    if(p->m_separators.find(address) != p->m_separators.end())
        return false;

    p->insert(address, ListingItemType::SeparatorItem);
    return true;
}

void ListingDocumentTypeNew::segment(const String &name, offset_t offset, address_t address, u64 size, SegmentType type) { this->segment(name, offset, address, size, size, type);  }

void ListingDocumentTypeNew::segment(const String &name, offset_t offset, address_t address, u64 psize, u64 vsize, SegmentType type)
{
    PIMPL_P(ListingDocumentTypeNew);

    if(p->m_segments.insert(name, offset, address, psize, vsize, type))
        p->insert(address, ListingItemType::SegmentItem);
    else
        r_ctx->log("Segment insertion failed @ " + name.hex());
}

void ListingDocumentTypeNew::function(address_t address, const String &name, tag_t tag) { PIMPL_P(ListingDocumentTypeNew); p->function(address, name, SymbolType::Function, tag); }
void ListingDocumentTypeNew::function(address_t address, tag_t tag) { this->function(address, SymbolTable::name(address, SymbolType::Function), tag); }

void ListingDocumentTypeNew::pointer(address_t address, SymbolType type, tag_t tag)
{
    PIMPL_P(ListingDocumentTypeNew);
    type |= SymbolType::Pointer;

    if(p->m_symbols.create(address, SymbolTable::name(address, type), type, tag))
        p->insert(address, ListingItemType::SymbolItem);
}

void ListingDocumentTypeNew::branch(address_t address, s64 direction, tag_t tag)
{
    PIMPL_P(ListingDocumentTypeNew);
    String name = String::hex(address);

    if(!direction) name = "infinite_loop_" + name;
    else name = "loc_" + name;

    if(p->m_symbols.create(address, name, SymbolType::Code, tag))
        p->insert(address, ListingItemType::SymbolItem);
}

void ListingDocumentTypeNew::label(address_t address, tag_t tag)
{
    PIMPL_P(ListingDocumentTypeNew);

    if(p->m_symbols.create(address, SymbolTable::name(address, SymbolType::Code), SymbolType::Code, tag))
        p->insert(address, ListingItemType::SymbolItem);
}

void ListingDocumentTypeNew::asciiString(address_t address, size_t len)
{
    PIMPL_P(ListingDocumentTypeNew);

    if(!p->m_symbols.create(address, SymbolTable::name(address, SymbolType::String), SymbolType::String))
        return;

    p->m_blocks.dataSize(address, len, BlockItemFlags::AsciiString);
    p->insert(address, ListingItemType::SymbolItem);
}

void ListingDocumentTypeNew::wideString(address_t address, size_t len)
{
    PIMPL_P(ListingDocumentTypeNew);

    if(!p->m_symbols.create(address, SymbolTable::name(address, SymbolType::WideString), SymbolType::WideString))
        return;

    p->m_blocks.dataSize(address, len, BlockItemFlags::WideString);
    p->insert(address, ListingItemType::SymbolItem);
}

void ListingDocumentTypeNew::instruction(const CachedInstruction& instruction)
{
    PIMPL_P(ListingDocumentTypeNew);
    p->m_blocks.codeSize(instruction->address, instruction->size);
    p->insert(instruction->address, ListingItemType::InstructionItem);
}
size_t ListingDocumentTypeNew::blocksCount() const { PIMPL_P(const ListingDocumentTypeNew); return p->m_blocks.size(); }
size_t ListingDocumentTypeNew::itemsCount() const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.size(); }
size_t ListingDocumentTypeNew::segmentsCount() const { PIMPL_P(const ListingDocumentTypeNew); return p->m_segments.size(); }
size_t ListingDocumentTypeNew::functionsCount() const { PIMPL_P(const ListingDocumentTypeNew); return p->m_functions.size(); }
size_t ListingDocumentTypeNew::symbolsCount() const { PIMPL_P(const ListingDocumentTypeNew); return p->m_symbols.size(); }
ListingItem ListingDocumentTypeNew::itemAt(size_t idx) { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.at(idx); }
const Segment* ListingDocumentTypeNew::segmentAt(size_t idx) { PIMPL_P(const ListingDocumentTypeNew); return p->m_segments.at(idx); }
CachedInstruction ListingDocumentTypeNew::instruction(address_t address) { PIMPL_P(ListingDocumentTypeNew); return p->m_instructions.find(address); }
address_location ListingDocumentTypeNew::function(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_functions.functionFromAddress(address); }
const Segment* ListingDocumentTypeNew::segment(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_segments.find(address); }
const Symbol* ListingDocumentTypeNew::symbol(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_symbols.symbol(address); }
const Symbol* ListingDocumentTypeNew::symbol(const String& name) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_symbols.symbol(name); }
size_t ListingDocumentTypeNew::itemSegmentIndex(address_t address, size_t index) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.segmentIndex(address, index); }
size_t ListingDocumentTypeNew::itemFunctionIndex(address_t address, size_t index) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.functionIndex(address, index); }
size_t ListingDocumentTypeNew::itemInstructionIndex(address_t address, size_t index) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.instructionIndex(address, index); }
size_t ListingDocumentTypeNew::itemSymbolIndex(address_t address, size_t index) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.symbolIndex(address, index); }
ListingItem ListingDocumentTypeNew::itemSegment(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.segmentItem(address); }
ListingItem ListingDocumentTypeNew::itemFunction(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.functionItem(address); }
ListingItem ListingDocumentTypeNew::itemInstruction(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.instructionItem(address); }
ListingItem ListingDocumentTypeNew::itemSymbol(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.symbolItem(address); }

void ListingDocumentTypeNew::autoComment(address_t address, const String& s)
{
    if(s.empty()) return;
    PIMPL_P(ListingDocumentTypeNew);

    auto it = p->m_itemdata[address].autocomments.insert(s);
    if(!it.second) return;

    size_t idx = this->itemInstructionIndex(address);
    if(idx == REDasm::npos) return;

    p->notify(idx);
}

CachedInstruction ListingDocumentTypeNew::cacheInstruction(address_t address) { PIMPL_P(ListingDocumentTypeNew); return p->m_instructions.allocate(address); }
bool ListingDocumentTypeNew::isInstructionCached(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_instructions.contains(address); }

void ListingDocumentTypeNew::moveToEntry()
{
    PIMPL_P(ListingDocumentTypeNew);

    if(!p->m_entry)
        return;

    p->m_cursor.set(this->itemFunctionIndex(p->m_entry->address));
}

void ListingDocumentTypeNew::setEntry(address_t address)
{
    PIMPL_P(ListingDocumentTypeNew);
    p->m_entry = p->m_symbols.symbol(address);
    p->m_cursor.set(p->m_items.functionIndex(address));
}

} // namespace REDasm
