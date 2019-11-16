#include "listingdocument.h"
#include <impl/disassembler/listing/document/listingdocument_impl.h>
#include <impl/support/utils_impl.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/context.h>

#define COMMENT_SEPARATOR " | "

namespace REDasm {

ListingDocumentType::ListingDocumentType(): Object(), m_pimpl_p(new ListingDocumentTypeImpl(this)) { }
const BlockContainer *ListingDocumentType::blocks() const { PIMPL_P(const ListingDocumentType); return p->blocks(); }
const ListingItems *ListingDocumentType::items() const { PIMPL_P(const ListingDocumentType); return p->items(); }
const ListingSegments *ListingDocumentType::segments() const { PIMPL_P(const ListingDocumentType); return p->segments(); }
const ListingFunctions *ListingDocumentType::functions() const { PIMPL_P(const ListingDocumentType); return p->functions(); }
const SymbolTable* ListingDocumentType::symbols() const { PIMPL_P(const ListingDocumentType); return p->symbols(); }
const ListingCursor& ListingDocumentType::cursor() const { PIMPL_P(const ListingDocumentType); return p->cursor(); }
ListingCursor& ListingDocumentType::cursor() { PIMPL_P(ListingDocumentType); return p->cursor(); }

const BlockItem* ListingDocumentType::entryBlock() const
{
   PIMPL_P(const ListingDocumentType);
   if(!p->m_entry) return nullptr;
   return this->block(p->m_entry->address);
}

const BlockItem* ListingDocumentType::blockAt(size_t idx) const { PIMPL_P(const ListingDocumentType); return p->m_blocks.at(idx); }
const BlockItem* ListingDocumentType::firstBlock() const { PIMPL_P(const ListingDocumentType); return p->m_blocks.first(); }
const BlockItem* ListingDocumentType::lastBlock() const { PIMPL_P(const ListingDocumentType); return p->m_blocks.last(); }

size_t ListingDocumentType::entryBlockIndex() const
{
   PIMPL_P(const ListingDocumentType);
   const BlockItem* bi = this->entryBlock();
   if(!bi) return REDasm::npos;
   return p->m_blocks.indexOf(bi);
}

const Symbol* ListingDocumentType::entry() const { PIMPL_P(const ListingDocumentType); return p->m_entry; }

void ListingDocumentType::entry(address_t address)
{
    PIMPL_P(ListingDocumentType);

    // String name;
    // Symbol* symbol = p->m_symbols.symbol(address); // Don't override custom symbols, if any

    // if(symbol)
    //     name = symbol->name;

    //p->function(address, name.empty() ? ENTRY_FUNCTION : name, SymbolType::FunctionNew, SymbolFlags::Export | SymbolFlags::EntryPoint);
    p->symbol(address, ENTRY_FUNCTION, SymbolType::FunctionNew, SymbolFlags::Export | SymbolFlags::EntryPoint);
    this->setEntry(address);
}

void ListingDocumentType::empty(address_t address) { PIMPL_P(ListingDocumentType); p->insert(address, ListingItemType::EmptyItem); }

bool ListingDocumentType::separator(address_t address)
{
    PIMPL_P(ListingDocumentType);

    if(p->m_separators.find(address) != p->m_separators.end())
        return false;

    p->insert(address, ListingItemType::SeparatorItem);
    return true;
}

void ListingDocumentType::segment(const String &name, offset_t offset, address_t address, u64 size, SegmentType type) { this->segment(name, offset, address, size, size, type);  }

void ListingDocumentType::segment(const String &name, offset_t offset, address_t address, u64 psize, u64 vsize, SegmentType type)
{
    PIMPL_P(ListingDocumentType);

    if(p->m_segments.insert(name, offset, address, psize, vsize, type))
    {
        p->unexplored(address, vsize);
        p->insert(address, ListingItemType::SegmentItem);
    }
    else
        r_ctx->log("Segment insertion failed @ " + name.hex());
}

void ListingDocumentType::meta(address_t address, const String& s, const String& name)
{
    PIMPL_P(ListingDocumentType);

    size_t index = 0;
    auto it = p->m_metas.find(address);

    if(it != p->m_metas.end()) index = ++it->second;
    else p->m_metas[address] = 0;

    if(name.empty()) p->m_itemdata[address].meta[index] = { ".meta", s };
    else p->m_itemdata[address].meta[index] = { "." + name, s };

    p->insert(address, ListingItemType::MetaItem, index);
}

void ListingDocumentType::type(address_t address, const String& s)
{
    PIMPL_P(ListingDocumentType);
    p->m_itemdata[address].type = s;

    this->empty(address);
    p->insert(address, ListingItemType::TypeItem);
}

void ListingDocumentType::table(address_t address, size_t count, tag_t tag)
{
    this->tableItem(address, address, 0, tag);
    this->type(address, "Table with " + String::number(count) + " case(s)");
}

void ListingDocumentType::tableItem(address_t address, address_t startaddress, u64 idx, tag_t tag)
{
    PIMPL_P(ListingDocumentType);

    p->block(address, r_asm->addressWidth(),
             SymbolTable::name(startaddress, SymbolType::Data, SymbolFlags::TableItem) + "_" + String::number(idx),
             SymbolType::Data, SymbolFlags::TableItem);
}

void ListingDocumentType::function(address_t address, const String &name, tag_t tag) { PIMPL_P(ListingDocumentType); p->symbol(address, name, SymbolType::FunctionNew, SymbolFlags::None, tag); }
void ListingDocumentType::function(address_t address, tag_t tag) { this->function(address, SymbolTable::name(address, SymbolType::FunctionNew), tag); }

void ListingDocumentType::pointer(address_t address, SymbolType type, tag_t tag)
{
    PIMPL_P(ListingDocumentType);
    p->block(address, r_asm->addressWidth(), type, SymbolFlags::Pointer);
}

void ListingDocumentType::pointer(address_t address, const String& name, SymbolType type, tag_t tag)
{
    PIMPL_P(ListingDocumentType);
    p->block(address, r_asm->addressWidth(), name, type, SymbolFlags::Pointer);
}

void ListingDocumentType::branch(address_t address, s64 direction, tag_t tag)
{
    PIMPL_P(ListingDocumentType);
    String name = String::hex(address);

    if(!direction) name = "infinite_loop_" + name;
    else name = "loc_" + name;

    p->symbol(address, name, SymbolType::LabelNew);
}

void ListingDocumentType::data(address_t address, size_t size) { this->data(address, size, String()); }

void ListingDocumentType::data(address_t address, size_t size, const String& name)
{
    if(size)
    {
        PIMPL_P(ListingDocumentType);
        p->block(address, size, name, SymbolType::DataNew);
    }
    else
        r_ctx->problem("Invalid data size @ " + String::hex(address));
}

void ListingDocumentType::label(address_t address) { PIMPL_P(ListingDocumentType); p->symbol(address, SymbolType::LabelNew); }
void ListingDocumentType::imported(address_t address, size_t size, const String& name) { PIMPL_P(ListingDocumentType); p->block(address, size, name, SymbolType::ImportNew); }
void ListingDocumentType::exported(address_t address, const String& name) { PIMPL_P(ListingDocumentType); p->symbol(address, name, SymbolType::DataNew, SymbolFlags::Export); }
void ListingDocumentType::exportedFunction(address_t address, const String& name) { PIMPL_P(ListingDocumentType); p->symbol(address, name, SymbolType::FunctionNew, SymbolFlags::Export); }
void ListingDocumentType::asciiString(address_t address, size_t size) { PIMPL_P(ListingDocumentType); p->block(address, size, SymbolType::StringNew, SymbolFlags::AsciiString); }
void ListingDocumentType::wideString(address_t address, size_t size) { PIMPL_P(ListingDocumentType); p->block(address, size, SymbolType::StringNew, SymbolFlags::WideString); }

void ListingDocumentType::instruction(const CachedInstruction& instruction)
{
    PIMPL_P(ListingDocumentType);
    p->m_instructions.cache(instruction);
    p->block(instruction);
}

size_t ListingDocumentType::blocksCount() const { PIMPL_P(const ListingDocumentType); return p->m_blocks.size(); }
size_t ListingDocumentType::itemsCount() const { PIMPL_P(const ListingDocumentType); return p->m_items.size(); }
size_t ListingDocumentType::segmentsCount() const { PIMPL_P(const ListingDocumentType); return p->m_segments.size(); }
size_t ListingDocumentType::functionsCount() const { PIMPL_P(const ListingDocumentType); return p->m_functions.size(); }
size_t ListingDocumentType::symbolsCount() const { PIMPL_P(const ListingDocumentType); return p->m_symbols.size(); }
bool ListingDocumentType::empty() const { PIMPL_P(const ListingDocumentType); return p->m_items.empty(); }
ListingItem& ListingDocumentType::itemAt(size_t idx) { PIMPL_P(ListingDocumentType); return p->m_items.at(idx); }
const ListingItem& ListingDocumentType::itemAt(size_t idx) const { PIMPL_P(const ListingDocumentType); return p->m_items.at(idx); }
Segment* ListingDocumentType::segment(address_t address) { PIMPL_P(ListingDocumentType); return p->m_segments.find(address); }
const Segment* ListingDocumentType::segmentAt(size_t idx) const { PIMPL_P(const ListingDocumentType); return p->m_segments.at(idx); }
address_t ListingDocumentType::functionAt(size_t idx) const { PIMPL_P(const ListingDocumentType); return p->m_functions.at(idx); }

ListingItem ListingDocumentType::currentItem() const
{
    PIMPL_P(const ListingDocumentType);
    if(p->m_cursor.currentLine() >= p->m_items.size()) return ListingItem();
    return p->m_items.at(p->m_cursor.currentLine());
}

CachedInstruction ListingDocumentType::instruction(address_t address) { PIMPL_P(ListingDocumentType); return p->m_instructions.find(address); }
address_location ListingDocumentType::function(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_functions.functionFromAddress(address); }
const Segment* ListingDocumentType::segment(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_segments.find(address); }
const Symbol* ListingDocumentType::symbol(address_t address) const { PIMPL_P(const ListingDocumentType); return p->symbol(address); }
const Symbol* ListingDocumentType::symbol(const String& name) const { PIMPL_P(const ListingDocumentType); return p->symbol(name); }
const BlockItem* ListingDocumentType::block(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_blocks.find(address); }

String ListingDocumentType::type(address_t address) const
{
    PIMPL_P(const ListingDocumentType);
    auto it = p->m_itemdata.find(address);
    return it != p->m_itemdata.end() ? it->second.type : String();
}

String ListingDocumentType::comment(address_t address, bool skipauto) const
{
    PIMPL_P(const ListingDocumentType);
    auto it = p->m_itemdata.find(address);
    if(it == p->m_itemdata.end()) return String();

    String cmt;
    ListingCommentSet comments = it->second.comments;

    if(!skipauto)
        comments.insert(it->second.autocomments.begin(), it->second.autocomments.end());

    return UtilsImpl::join(comments, COMMENT_SEPARATOR);
}

size_t ListingDocumentType::itemIndex(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_items.itemIndex(address); }
size_t ListingDocumentType::itemListingIndex(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_items.listingIndex(address); }
size_t ListingDocumentType::itemSegmentIndex(address_t address, size_t index) const { PIMPL_P(const ListingDocumentType); return p->m_items.segmentIndex(address, index); }
size_t ListingDocumentType::itemFunctionIndex(address_t address, size_t index) const { PIMPL_P(const ListingDocumentType); return p->m_items.functionIndex(address, index); }
size_t ListingDocumentType::itemInstructionIndex(address_t address, size_t index) const { PIMPL_P(const ListingDocumentType); return p->m_items.instructionIndex(address, index); }
size_t ListingDocumentType::itemSymbolIndex(address_t address, size_t index) const { PIMPL_P(const ListingDocumentType); return p->m_items.symbolIndex(address, index); }
ListingItem ListingDocumentType::itemListing(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_items.listingItem(address);  }
ListingItem ListingDocumentType::itemSegment(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_items.segmentItem(address); }
ListingItem ListingDocumentType::itemFunction(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_items.functionItem(address); }
ListingItem ListingDocumentType::itemInstruction(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_items.instructionItem(address); }
ListingItem ListingDocumentType::itemSymbol(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_items.symbolItem(address); }

void ListingDocumentType::autoComment(address_t address, const String& s)
{
    if(s.empty()) return;
    PIMPL_P(ListingDocumentType);

    auto it = p->m_itemdata[address].autocomments.insert(s);
    if(!it.second) return;

    size_t idx = this->itemInstructionIndex(address);
    if(idx == REDasm::npos) return;

    p->notify(idx);
}

void ListingDocumentType::comment(address_t address, const String& s)
{
    if(s.empty()) return;
    PIMPL_P(ListingDocumentType);

    auto it = p->m_itemdata[address].comments.insert(s);
    if(!it.second) return;

    size_t idx = this->itemInstructionIndex(address);
    if(idx == REDasm::npos) return;

    p->notify(idx);
}

CachedInstruction ListingDocumentType::allocateInstruction() { PIMPL_P(ListingDocumentType); return p->m_instructions.allocate(); }

size_t ListingDocumentType::entryInstructionIndex() const
{
   PIMPL_P(const ListingDocumentType);
   if(!p->m_entry) return REDasm::npos;
   return this->itemInstructionIndex(p->m_entry->address);
}

CachedInstruction ListingDocumentType::entryInstruction()
{
   PIMPL_P(const ListingDocumentType);
   if(!p->m_entry) return CachedInstruction();
   return this->instruction(p->m_entry->address);
}

bool ListingDocumentType::isInstructionCached(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_instructions.contains(address); }
bool ListingDocumentType::rename(address_t address, const String& name) { PIMPL_P(ListingDocumentType); return p->rename(address, name); }

const Symbol* ListingDocumentType::functionStartSymbol(address_t address) const
{
    PIMPL_P(const ListingDocumentType);
    auto location = p->m_functions.functionFromAddress(address);
    if(!location.valid) return nullptr;
    return p->symbol(location);
}

ListingItem ListingDocumentType::functionStart(address_t address) const { PIMPL_P(const ListingDocumentType); return p->functionStart(address); }
const FunctionGraph* ListingDocumentType::graph(address_t address) const { PIMPL_P(const ListingDocumentType); return p->graph(address); }
FunctionGraph* ListingDocumentType::graph(address_t address) { PIMPL_P(ListingDocumentType); return p->graph(address);  }
void ListingDocumentType::graph(address_t address, FunctionGraph* graph) { PIMPL_P(ListingDocumentType); return p->graph(address, graph); }
void ListingDocumentType::segmentCoverage(address_t address, size_t coverage) { PIMPL_P(ListingDocumentType); p->segmentCoverage(address, coverage);  }
void ListingDocumentType::segmentCoverageAt(size_t idx, size_t coverage) { PIMPL_P(ListingDocumentType); p->segmentCoverageAt(idx, coverage); }
void ListingDocumentType::invalidateGraphs() { PIMPL_P(ListingDocumentType); return p->invalidateGraphs(); }

bool ListingDocumentType::next(const BlockItem*& item) const
{
    if(!item || (item == this->lastBlock())) return false;

    PIMPL_P(const ListingDocumentType);
    size_t idx = p->m_blocks.indexOf(item);
    if(idx == REDasm::npos) return false;
    item = p->m_blocks.at(++idx);
    return true;
}

bool ListingDocumentType::next(CachedInstruction& item)
{
    const BlockItem* bi = this->block(item->address);
    if(!this->next(bi)) return false;

    PIMPL_P(ListingDocumentType);
    item = p->m_instructions.find(bi->start);
    return item;
}

void ListingDocumentType::moveToEntry()
{
    PIMPL_P(ListingDocumentType);
    if(!p->m_entry) return;
    p->m_cursor.set(this->itemFunctionIndex(p->m_entry->address));
}

void ListingDocumentType::setEntry(address_t address)
{
    PIMPL_P(ListingDocumentType);
    p->m_entry = p->m_symbols.get(address);
    p->m_cursor.set(p->m_items.functionIndex(address));
}

bool ListingDocumentType::goTo(const ListingItem& item)
{
    if(!item.isValid()) return false;

    PIMPL_P(ListingDocumentType);
    size_t idx = p->m_items.indexOf(item);
    if(idx == REDasm::npos) return false;
    p->m_cursor.moveTo(idx);
    return true;
}

bool ListingDocumentType::goTo(address_t address)
{
    size_t idx = this->itemSymbolIndex(address);
    if(idx == REDasm::npos) idx = this->itemInstructionIndex(address);
    if(idx == REDasm::npos) return false;

    PIMPL_P(ListingDocumentType);
    return this->goTo(p->m_items.at(idx));
}

} // namespace REDasm
