#include "listingdocumentnew.h"
#include <impl/disassembler/listing/listingdocumentnew_impl.h>
#include <impl/support/utils_impl.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/context.h>

#define COMMENT_SEPARATOR " | "

namespace REDasm {

ListingDocumentTypeNew::ListingDocumentTypeNew(): Object(), m_pimpl_p(new ListingDocumentTypeNewImpl(this)) { }
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

    // String name;
    // Symbol* symbol = p->m_symbols.symbol(address); // Don't override custom symbols, if any

    // if(symbol)
    //     name = symbol->name;

    //p->function(address, name.empty() ? ENTRY_FUNCTION : name, SymbolType::FunctionNew, SymbolFlags::Export | SymbolFlags::EntryPoint);
    p->symbol(address, ENTRY_FUNCTION, SymbolType::FunctionNew, SymbolFlags::Export | SymbolFlags::EntryPoint);
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
    {
        p->unexplored(address, vsize);
        p->insert(address, ListingItemType::SegmentItem);
    }
    else
        r_ctx->log("Segment insertion failed @ " + name.hex());
}

void ListingDocumentTypeNew::type(address_t address, const String& s)
{
    PIMPL_P(ListingDocumentTypeNew);
    p->m_itemdata[address].type = s;

    this->empty(address);
    p->insert(address, ListingItemType::TypeItem);
}

void ListingDocumentTypeNew::table(address_t address, size_t count, tag_t tag)
{
    this->tableItem(address, address, 0, tag);
    this->type(address, "Table with " + String::number(count) + " case(s)");
}

void ListingDocumentTypeNew::tableItem(address_t address, address_t startaddress, u64 idx, tag_t tag)
{
    PIMPL_P(ListingDocumentTypeNew);

    p->block(address, r_asm->addressWidth(),
             SymbolTable::name(startaddress, SymbolType::Data, SymbolFlags::TableItem) + "_" + String::number(idx),
             SymbolType::Data, SymbolFlags::TableItem);
}

void ListingDocumentTypeNew::function(address_t address, const String &name, tag_t tag) { PIMPL_P(ListingDocumentTypeNew); p->symbol(address, name, SymbolType::FunctionNew, SymbolFlags::None, tag); }
void ListingDocumentTypeNew::function(address_t address, tag_t tag) { this->function(address, SymbolTable::name(address, SymbolType::FunctionNew), tag); }

void ListingDocumentTypeNew::pointer(address_t address, SymbolType type, tag_t tag)
{
    PIMPL_P(ListingDocumentTypeNew);
    p->block(address, r_asm->addressWidth(), type, SymbolFlags::Pointer);
}

void ListingDocumentTypeNew::branch(address_t address, s64 direction, tag_t tag)
{
    PIMPL_P(ListingDocumentTypeNew);
    String name = String::hex(address);

    if(!direction) name = "infinite_loop_" + name;
    else name = "loc_" + name;

    p->symbol(address, name, SymbolType::LabelNew);
}

void ListingDocumentTypeNew::data(address_t address, size_t size)
{
    if(size)
    {
        PIMPL_P(ListingDocumentTypeNew);
        p->block(address, size, SymbolType::DataNew);
    }
    else
        r_ctx->problem("Invalid data size @ " + String::hex(address));
}

void ListingDocumentTypeNew::label(address_t address) { PIMPL_P(ListingDocumentTypeNew); p->symbol(address, SymbolType::LabelNew); }
void ListingDocumentTypeNew::imported(address_t address, size_t size, const String& name) { PIMPL_P(ListingDocumentTypeNew); p->block(address, size, name, SymbolType::ImportNew); }
void ListingDocumentTypeNew::exported(address_t address, const String& name) { PIMPL_P(ListingDocumentTypeNew); p->symbol(address, name, SymbolType::DataNew, SymbolFlags::Export); }
void ListingDocumentTypeNew::exportedFunction(address_t address, const String& name) { PIMPL_P(ListingDocumentTypeNew); p->symbol(address, name, SymbolType::FunctionNew, SymbolFlags::Export); }
void ListingDocumentTypeNew::asciiString(address_t address, size_t size) { PIMPL_P(ListingDocumentTypeNew); p->block(address, size, SymbolType::StringNew, SymbolFlags::AsciiString); }
void ListingDocumentTypeNew::wideString(address_t address, size_t size) { PIMPL_P(ListingDocumentTypeNew); p->block(address, size, SymbolType::StringNew, SymbolFlags::WideString); }
void ListingDocumentTypeNew::instruction(const CachedInstruction& instruction) { PIMPL_P(ListingDocumentTypeNew); p->block(instruction); }
size_t ListingDocumentTypeNew::blocksCount() const { PIMPL_P(const ListingDocumentTypeNew); return p->m_blocks.size(); }
size_t ListingDocumentTypeNew::itemsCount() const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.size(); }
size_t ListingDocumentTypeNew::segmentsCount() const { PIMPL_P(const ListingDocumentTypeNew); return p->m_segments.size(); }
size_t ListingDocumentTypeNew::functionsCount() const { PIMPL_P(const ListingDocumentTypeNew); return p->m_functions.size(); }
size_t ListingDocumentTypeNew::symbolsCount() const { PIMPL_P(const ListingDocumentTypeNew); return p->m_symbols.size(); }
bool ListingDocumentTypeNew::empty() const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.empty(); }
ListingItem& ListingDocumentTypeNew::itemAt(size_t idx) { PIMPL_P(ListingDocumentTypeNew); return p->m_items.at(idx); }
const ListingItem& ListingDocumentTypeNew::itemAt(size_t idx) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.at(idx); }
Segment* ListingDocumentTypeNew::segment(address_t address) { PIMPL_P(ListingDocumentTypeNew); return p->m_segments.find(address); }
const Segment* ListingDocumentTypeNew::segmentAt(size_t idx) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_segments.at(idx); }

ListingItem ListingDocumentTypeNew::currentItem() const
{
    PIMPL_P(const ListingDocumentTypeNew);
    if(p->m_cursor.currentLine() >= p->m_items.size()) return ListingItem();
    return p->m_items.at(p->m_cursor.currentLine());
}

CachedInstruction ListingDocumentTypeNew::instruction(address_t address) { PIMPL_P(ListingDocumentTypeNew); return p->m_instructions.find(address); }
address_location ListingDocumentTypeNew::function(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_functions.functionFromAddress(address); }
const Segment* ListingDocumentTypeNew::segment(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_segments.find(address); }
const Symbol* ListingDocumentTypeNew::symbol(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->symbol(address); }
const Symbol* ListingDocumentTypeNew::symbol(const String& name) const { PIMPL_P(const ListingDocumentTypeNew); return p->symbol(name); }
const BlockItem* ListingDocumentTypeNew::block(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_blocks.find(address); }

String ListingDocumentTypeNew::type(address_t address) const
{
    PIMPL_P(const ListingDocumentTypeNew);
    auto it = p->m_itemdata.find(address);
    return it != p->m_itemdata.end() ? it->second.type : String();
}

String ListingDocumentTypeNew::comment(address_t address, bool skipauto) const
{
    PIMPL_P(const ListingDocumentTypeNew);
    auto it = p->m_itemdata.find(address);
    if(it == p->m_itemdata.end()) return String();

    String cmt;
    ListingCommentSet comments = it->second.comments;

    if(!skipauto)
        comments.insert(it->second.autocomments.begin(), it->second.autocomments.end());

    return UtilsImpl::join(comments, COMMENT_SEPARATOR);
}

size_t ListingDocumentTypeNew::itemIndex(address_t address) { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.itemIndex(address); }
size_t ListingDocumentTypeNew::itemListingIndex(address_t address) { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.listingIndex(address); }
size_t ListingDocumentTypeNew::itemSegmentIndex(address_t address, size_t index) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.segmentIndex(address, index); }
size_t ListingDocumentTypeNew::itemFunctionIndex(address_t address, size_t index) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.functionIndex(address, index); }
size_t ListingDocumentTypeNew::itemInstructionIndex(address_t address, size_t index) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.instructionIndex(address, index); }
size_t ListingDocumentTypeNew::itemSymbolIndex(address_t address, size_t index) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.symbolIndex(address, index); }
ListingItem ListingDocumentTypeNew::itemListing(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_items.listingItem(address);  }
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

void ListingDocumentTypeNew::comment(address_t address, const String& s)
{
    if(s.empty()) return;
    PIMPL_P(ListingDocumentTypeNew);

    auto it = p->m_itemdata[address].comments.insert(s);
    if(!it.second) return;

    size_t idx = this->itemInstructionIndex(address);
    if(idx == REDasm::npos) return;

    p->notify(idx);
}

CachedInstruction ListingDocumentTypeNew::cacheInstruction(address_t address) { PIMPL_P(ListingDocumentTypeNew); return p->m_instructions.allocate(address); }
bool ListingDocumentTypeNew::isInstructionCached(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->m_instructions.contains(address); }
bool ListingDocumentTypeNew::rename(address_t address, const String& name) { PIMPL_P(ListingDocumentTypeNew); return p->rename(address, name); }

const Symbol* ListingDocumentTypeNew::functionStartSymbol(address_t address) const
{
    PIMPL_P(const ListingDocumentTypeNew);
    auto location = p->m_functions.functionFromAddress(address);
    if(!location.valid) return nullptr;
    return p->symbol(location);
}

ListingItem ListingDocumentTypeNew::functionStart(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->functionStart(address); }
const FunctionGraph* ListingDocumentTypeNew::graph(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->graph(address); }
FunctionGraph* ListingDocumentTypeNew::graph(address_t address) { PIMPL_P(ListingDocumentTypeNew); return p->graph(address);  }
void ListingDocumentTypeNew::graph(address_t address, FunctionGraph* graph) { PIMPL_P(ListingDocumentTypeNew); return p->graph(address, graph); }
void ListingDocumentTypeNew::segmentCoverage(address_t address, size_t coverage) { PIMPL_P(ListingDocumentTypeNew); p->segmentCoverage(address, coverage);  }
void ListingDocumentTypeNew::segmentCoverageAt(size_t idx, size_t coverage) { PIMPL_P(ListingDocumentTypeNew); p->segmentCoverageAt(idx, coverage); }
void ListingDocumentTypeNew::invalidateGraphs() { PIMPL_P(ListingDocumentTypeNew); return p->invalidateGraphs(); }

void ListingDocumentTypeNew::moveToEntry()
{
    PIMPL_P(ListingDocumentTypeNew);
    if(!p->m_entry) return;
    p->m_cursor.set(this->itemFunctionIndex(p->m_entry->address));
}

void ListingDocumentTypeNew::setEntry(address_t address)
{
    PIMPL_P(ListingDocumentTypeNew);
    p->m_entry = p->m_symbols.get(address);
    p->m_cursor.set(p->m_items.functionIndex(address));
}

bool ListingDocumentTypeNew::canSymbolizeAddress(address_t address) const { PIMPL_P(const ListingDocumentTypeNew); return p->canSymbolizeAddress(address); }

bool ListingDocumentTypeNew::goTo(const ListingItem& item)
{
    if(!item.isValid()) return false;

    PIMPL_P(ListingDocumentTypeNew);
    size_t idx = p->m_items.indexOf(item);
    if(idx == REDasm::npos) return false;
    p->m_cursor.moveTo(idx);
    return true;
}

bool ListingDocumentTypeNew::goTo(address_t address)
{
    size_t idx = this->itemSymbolIndex(address);
    if(idx == REDasm::npos) idx = this->itemInstructionIndex(address);
    if(idx == REDasm::npos) return false;

    PIMPL_P(ListingDocumentTypeNew);
    return this->goTo(p->m_items.at(idx));
}

} // namespace REDasm
