#include "listingdocument.h"
#include "../../support/utils.h"
#include <impl/support/utils_impl.h>
#include <impl/disassembler/listing/listingdocument_impl.h>
#include <algorithm>
#include <sstream>

#define COMMENT_SEPARATOR " | "

namespace REDasm {

ListingDocumentChangedEventArgs::ListingDocumentChangedEventArgs(const ListingItem *item, size_t index, ListingDocumentAction action): EventArgs(new ListingDocumentChangedEventArgsImpl(item, index, action)) { }
ListingDocumentChangedEventArgs::ListingDocumentChangedEventArgs(const ListingItem& item, size_t index, ListingDocumentAction action): EventArgs(new ListingDocumentChangedEventArgsImpl(item, index, action)) { }
const ListingItem *ListingDocumentChangedEventArgs::item() const { PIMPL_P(const ListingDocumentChangedEventArgs); return p->item(); }
const ListingItem& ListingDocumentChangedEventArgs::itemNew() const { PIMPL_P(const ListingDocumentChangedEventArgs); return p->itemNew(); }
ListingDocumentAction ListingDocumentChangedEventArgs::action() const { PIMPL_P(const ListingDocumentChangedEventArgs); return p->action(); }
bool ListingDocumentChangedEventArgs::isInserted() const { PIMPL_P(const ListingDocumentChangedEventArgs); return p->isInserted(); }
bool ListingDocumentChangedEventArgs::isRemoved() const { PIMPL_P(const ListingDocumentChangedEventArgs); return p->isRemoved(); }
size_t ListingDocumentChangedEventArgs::index() const { PIMPL_P(const ListingDocumentChangedEventArgs); return p->index(); }

ListingDocumentType::ListingDocumentType(): m_pimpl_p(new ListingDocumentTypeImpl(this)) { }
void ListingDocumentType::save(cereal::BinaryOutputArchive &a) const { PIMPL_P(const ListingDocumentType); return p->save(a); }
void ListingDocumentType::load(cereal::BinaryInputArchive &a) { PIMPL_P(ListingDocumentType); return p->load(a); }
size_t ListingDocumentType::size() const { PIMPL_P(const ListingDocumentType); return p->size(); }
bool ListingDocumentType::empty() const { PIMPL_P(const ListingDocumentType); return p->empty(); }

bool ListingDocumentType::advance(CachedInstruction &instruction)
{
    if(!instruction)
        return false;

    instruction = this->instruction(instruction->endAddress());
    return instruction && !instruction->isInvalid();
}

const ListingCursor *ListingDocumentType::cursor() const { PIMPL_P(const ListingDocumentType); return &p->m_cursor; }
ListingCursor *ListingDocumentType::cursor() { PIMPL_P(ListingDocumentType); return &p->m_cursor; }

bool ListingDocumentType::goTo(const ListingItem *item)
{
    if(!item)
        return false;

    PIMPL_P(ListingDocumentType);
    size_t idx = p->findIndex(item->address_new, item->type_new);

    if(idx == REDasm::npos)
        return false;

    p->m_cursor.moveTo(idx);
    return true;
}

bool ListingDocumentType::goTo(address_t address)
{
    PIMPL_P(ListingDocumentType);

    auto it = p->symbolIterator(address);

    if(it == p->end())
        it = p->instructionIterator(address);

    if(it == p->end())
        return false;

    this->goTo(it->get());
    return true;
}

void ListingDocumentType::moveToEP()
{
    PIMPL_P(ListingDocumentType);

    if(!p->m_documententry)
        return;

    p->m_cursor.set(this->functionIndex(p->m_documententry->address));
}

size_t ListingDocumentType::lastLine() const { return this->size() - 1; }
ListingFunctions* ListingDocumentType::functions() { PIMPL_P(ListingDocumentType); return &p->m_functions; }
CachedInstruction ListingDocumentType::cacheInstruction(address_t address) { PIMPL_P(ListingDocumentType); return p->m_instructions.allocate(address); }
const ListingFunctions* ListingDocumentType::functions() const { PIMPL_P(const ListingDocumentType); return &p->m_functions; }
const List &ListingDocumentType::segments() const { PIMPL_P(const ListingDocumentType); return p->m_segments; }

String ListingDocumentType::comment(ListingItem* item, bool skipauto) const
{
    String cmt;
    ListingCommentSet comments = item->data()->comments;

    if(!skipauto)
        comments.insert(item->data()->autocomments.begin(), item->data()->autocomments.end());

    return UtilsImpl::join(comments, COMMENT_SEPARATOR);
}

void ListingDocumentType::comment(ListingItem *item, const String &s)
{
    if(!s.empty())
        item->data()->comments.insert(s.simplified());
    else
        item->data()->comments.clear();

    ListingDocumentChangedEventArgs ldc(item, this->itemIndex(item));
    changed(&ldc);
}

ListingItem *ListingDocumentType::functionStart(ListingItem *item) const
{
    if(!item)
        return nullptr;

    if(item->is(ListingItemType::FunctionItem))
        return item;

    PIMPL_P(const ListingDocumentType);
    return this->functionItem(p->m_functions.functionFromAddress(item->address_new));
}

ListingItem *ListingDocumentType::functionStart(address_t address) const
{
    ListingItem* item = this->instructionItem(address);

    if(!item)
        return nullptr;

    PIMPL_P(const ListingDocumentType);
    return this->functionItem(p->m_functions.functionFromAddress(item->address_new));
}

ListingItem *ListingDocumentType::currentFunction() const
{
    if(!this->currentItem())
        return nullptr;

    return this->functionStart(this->currentItem());
}

ListingItem *ListingDocumentType::currentItem() const
{
    PIMPL_P(const ListingDocumentType);

    if(p->m_cursor.currentLine() >= this->size())
        return nullptr;

    return this->itemAt(p->m_cursor.currentLine());
}

ListingItem *ListingDocumentType::segmentItem(address_t address) const
{
    PIMPL_P(const ListingDocumentType);
    auto it = p->segmentIterator(address);

    if(it == p->end())
        return nullptr;

    return it->get();
}

ListingItem *ListingDocumentType::symbolItem(address_t address) const
{
    PIMPL_P(const ListingDocumentType);
    auto it = p->symbolIterator(address);

    if(it == p->end())
        return nullptr;

    return it->get();
}

ListingItem *ListingDocumentType::instructionItem(address_t address) const
{
    PIMPL_P(const ListingDocumentType);
    auto it = p->instructionIterator(address);

    if(it == p->end())
        return nullptr;

    return it->get();
}

ListingItem *ListingDocumentType::functionItem(address_t address) const
{
    PIMPL_P(const ListingDocumentType);
    auto it = p->functionIterator(address);

    if(it == p->end())
        return nullptr;

    return it->get();
}

Symbol* ListingDocumentType::functionStartSymbol(address_t address)
{
    const ListingItem* item = this->functionStart(address);

    if(item)
        return this->symbol(item->address_new);

    return nullptr;
}

CachedInstruction ListingDocumentType::entryInstruction()
{
    PIMPL_P(const ListingDocumentType);

    if(!p->m_documententry)
        return CachedInstruction();

    return this->instruction(p->m_documententry->address);
}

bool ListingDocumentType::isInstructionCached(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_instructions.contains(address); }
const ListingMetaItem &ListingDocumentType::meta(ListingItem* item) const { return item->data()->meta; }
String ListingDocumentType::type(ListingItem* item) const { return item->data()->type; }
void ListingDocumentType::empty(address_t address) { PIMPL_P(ListingDocumentType); p->push(address, ListingItemType::EmptyItem); }

bool ListingDocumentType::separator(address_t address)
{
    PIMPL_P(ListingDocumentType);

    // Check for duplicate separators
    if(p->findIndex(address, ListingItemType::SeparatorItem) != REDasm::npos)
        return false;

    p->push(address, ListingItemType::SeparatorItem);
    return true;
}

void ListingDocumentType::meta(address_t address, const String &s, const String &name)
{
    PIMPL_P(ListingDocumentType);

    size_t index = 0;
    auto it = p->m_activemeta.find(address);

    if(it != p->m_activemeta.end())
        index = ++it->second;
    else
        p->m_activemeta[address] = 0;

    ListingItem* item = p->push(address, ListingItemType::MetaItem, index);

    if(name.empty())
        item->data()->meta = { ".meta", s };
    else
        item->data()->meta = { "." + name, s };
}

void ListingDocumentType::type(address_t address, const String &s)
{
    PIMPL_P(ListingDocumentType);

    this->empty(address);
    ListingItem* item = p->push(address, ListingItemType::TypeItem);
    item->data()->type = s;
}

void ListingDocumentType::autoComment(address_t address, const String &s)
{
    if(s.empty())
        return;

    PIMPL_P(ListingDocumentType);
    auto it = p->instructionIterator(address);

    if(it == p->end())
    {
        p->m_pendingautocomments[address].insert(s);
        return;
    }

    it->get()->data()->autocomments.insert(s);

    ListingDocumentChangedEventArgs ldc(it->get(), this->itemIndex(it->get()));
    changed(&ldc);
}

void ListingDocumentType::branch(address_t address, s64 direction, tag_t tag)
{
    String name = String::hex(address);

    if(!direction)
        name = "infinite_loop_" + name;
    else
        name = "loc_" + name;

    this->symbol(address, name, SymbolType::Code, tag);
}

void ListingDocumentType::symbol(address_t address, const String &name, SymbolType type, tag_t tag)
{
    PIMPL_P(ListingDocumentType);
    Symbol* symbol = p->m_symboltable.get(address);

    if(symbol)
    {
        if(symbol->isLocked() && !(type & SymbolType::Locked))
            return;

        p->pop(address, ListingItemType::EmptyItem);
        p->pop(address, ListingItemType::MetaItem);

        if(symbol->isFunction())
        {
            p->push(address, ListingItemType::EmptyItem);
            p->pop(address, ListingItemType::FunctionItem);
        }
        else
            p->pop(address, ListingItemType::SymbolItem);

        p->m_symboltable.erase(address);
    }

    if(!this->segment(address))
        return;

    p->m_symboltable.create(address, SymbolTable::normalized(name), type, tag);

    if(type & SymbolType::FunctionMask)
    {
        p->push(address, ListingItemType::EmptyItem);
        p->push(address, ListingItemType::FunctionItem);
    }
    else
        p->push(address, ListingItemType::SymbolItem);
}

void ListingDocumentType::symbol(address_t address, SymbolType type, tag_t tag) { this->symbol(address, SymbolTable::name(address, type), type, tag); }

void ListingDocumentType::rename(address_t address, const String &name)
{
    if(name.empty())
        return;

    const Symbol* symbol = this->symbol(address);

    if(!symbol)
        return;

    this->symbol(address, name, symbol->type, symbol->tag);
}

void ListingDocumentType::lock(address_t address, const String &name)
{
    PIMPL_P(ListingDocumentType);
    const Symbol* symbol = p->m_symboltable.get(address);

    if(!symbol)
        this->lock(address, name.empty() ? symbol->name : name, SymbolType::Data);
    else
        this->lock(address, name.empty() ? symbol->name : name, symbol->type, symbol->tag);
}

void ListingDocumentType::lock(address_t address, SymbolType type, tag_t tag) { this->symbol(address, type | SymbolType::Locked, tag); }
void ListingDocumentType::lock(address_t address, const String &name, SymbolType type, tag_t tag) { this->symbol(address, name, type | SymbolType::Locked, tag); }
void ListingDocumentType::segment(const String &name, offset_t offset, address_t address, u64 size, SegmentType type) { this->segment(name, offset, address, size, size, type); }

void ListingDocumentType::segment(const String &name, offset_t offset, address_t address, u64 psize, u64 vsize, SegmentType type)
{
    if(!psize && !vsize)
    {
        r_ctx->log("Skipping empty segment " + name.quoted());
        return;
    }

    PIMPL_P(ListingDocumentType);

    for(size_t i = 0; i < p->m_segments.size(); i++)
    {
        Segment* segment = variant_object<Segment>(p->m_segments[i]);

        if(segment->is(SegmentType::Bss) ? segment->contains(address) : ((segment->offset == offset) || segment->contains(address)))
        {
            r_ctx->problem("Segment " + name.quoted() + " overlaps " + segment->name.quoted());
            return;
        }
    }

    p->m_segments.append(new Segment(name, offset, address, psize, vsize, type));

    p->m_segments.sort([](const Variant& v1, const Variant& v2) -> bool {
        Segment* s1 = variant_object<Segment>(v1);
        Segment* s2 = variant_object<Segment>(v2);
        return s1->address < s2->address;
    });

    p->push(address, ListingItemType::SegmentItem);
}

void ListingDocumentType::lockFunction(address_t address, const String &name, u32 tag) { this->lock(address, name, SymbolType::Function, tag);  }
void ListingDocumentType::function(address_t address, const String &name, tag_t tag) { this->symbol(address, name, SymbolType::Function, tag); }
void ListingDocumentType::function(address_t address, tag_t tag) { this->symbol(address, SymbolType::Function, tag); }
void ListingDocumentType::pointer(address_t address, SymbolType type, tag_t tag) { this->symbol(address, type | SymbolType::Pointer, tag); }

void ListingDocumentType::table(address_t address, size_t count, tag_t tag)
{
    this->lock(address, SymbolTable::name(address, SymbolType::TableItem) + "_0", SymbolType::TableItem, tag);
    this->type(address, "Table with " + String::number(count) + " case(s)");
}

void ListingDocumentType::tableItem(address_t address, address_t startaddress, u64 idx, tag_t tag)
{
    Symbol* symbol = this->symbol(address); // Don't override custom symbols, if any

    if(symbol)
    {
        symbol->type |= SymbolType::TableItem;
        this->lock(address, symbol->name, symbol->type, tag);
        return;
    }

    this->lock(address,
               SymbolTable::name(startaddress, SymbolType::TableItem) + "_" + String::number(idx),
               SymbolType::TableItem, tag);
}

void ListingDocumentType::entry(address_t address, tag_t tag)
{
    const Symbol* symep = this->symbol(address); // Don't override custom symbols, if any
    this->lock(address, symep ? symep->name : ENTRY_FUNCTION, SymbolType::EntryPoint, tag);
    this->setDocumentEntry(address);
}

void ListingDocumentType::eraseSymbol(address_t address)
{
    PIMPL_P(ListingDocumentType);
    p->pop(address, ListingItemType::SymbolItem);
    p->m_symboltable.erase(address);
}

void ListingDocumentType::setDocumentEntry(address_t address)
{
    PIMPL_P(ListingDocumentType);
    p->m_documententry = p->m_symboltable.get(address);
    p->m_cursor.set(this->functionIndex(address));
}

const Symbol *ListingDocumentType::documentEntry() const { PIMPL_P(const ListingDocumentType); return p->m_documententry; }
size_t ListingDocumentType::segmentsCount() const { PIMPL_P(const ListingDocumentType); return p->m_segments.size(); }

Segment *ListingDocumentType::segment(address_t address)
{
    PIMPL_P(ListingDocumentType);

    for(size_t i = 0; i < p->m_segments.size(); i++)
    {
        Segment* segment = variant_object<Segment>(p->m_segments[i]);

        if(segment->contains(address))
            return segment;
    }

    return nullptr;
}

const Segment *ListingDocumentType::segment(address_t address) const { return const_cast<ListingDocumentType*>(this)->segment(address); }

const Segment *ListingDocumentType::segmentByName(const String &name) const
{
    PIMPL_P(const ListingDocumentType);

    for(size_t i = 0; i < p->m_segments.size(); i++)
    {
        Segment* segment = variant_object<Segment>(p->m_segments[i]);

        if(segment->name == name)
            return segment;
    }

    return nullptr;
}

void ListingDocumentType::instruction(const CachedInstruction &instruction) { PIMPL_P(ListingDocumentType); p->push(instruction->address, ListingItemType::InstructionItem); }
CachedInstruction ListingDocumentType::instruction(address_t address) { PIMPL_P(ListingDocumentType); return p->m_instructions.find(address); }
CachedInstruction ListingDocumentType::nextInstruction(const CachedInstruction &instruction) { PIMPL_P(ListingDocumentType); return p->m_instructions.next(instruction->address); }
CachedInstruction ListingDocumentType::prevInstruction(const CachedInstruction &instruction) { PIMPL_P(ListingDocumentType); return p->m_instructions.prev(instruction->address); }
CachedInstruction ListingDocumentType::nearestInstruction(address_t address) { PIMPL_P(ListingDocumentType); return p->m_instructions.findNearest(address); }
size_t ListingDocumentType::itemIndex(const ListingItem *item) const { PIMPL_P(const ListingDocumentType); return p->findIndex(item->address_new, item->type_new, item->index_new); }
size_t ListingDocumentType::functionIndex(address_t address) const { PIMPL_P(const ListingDocumentType); return p->findIndex(address, ListingItemType::FunctionItem); }
size_t ListingDocumentType::instructionIndex(address_t address) const { PIMPL_P(const ListingDocumentType); return p->findIndex(address, ListingItemType::InstructionItem); }
size_t ListingDocumentType::symbolIndex(address_t address) const { PIMPL_P(const ListingDocumentType); return p->findIndex(address, ListingItemType::SymbolItem); }

ListingItem* ListingDocumentType::itemAt(size_t i) const
{
    if(i >= this->size())
        return nullptr;

    PIMPL_P(const ListingDocumentType);
    return p->at(i).get();
}

ListingItem *ListingDocumentType::next(ListingItem *item) const
{
    PIMPL_P(const ListingDocumentType);
    auto it = p->findIterator(item);

    if(it != p->end())
        it++;

    return (it != p->end()) ? it->get() : nullptr;
}

ListingItem *ListingDocumentType::prev(ListingItem *item) const
{
    PIMPL_P(const ListingDocumentType);
    auto it = p->findIterator(item);

    if((it == p->begin()) || (it == p->end()))
        return nullptr;

    it--;
    return it->get();
}

ListingItem *ListingDocumentType::nextInstructionItem(ListingItem *item)
{
    PIMPL_P(ListingDocumentType);
    address_location loc = p->m_instructions.nextHint(item->address_new);
    return loc.valid ? this->instructionItem(loc) : nullptr;
}

ListingItem *ListingDocumentType::prevInstructionItem(ListingItem *item)
{
    PIMPL_P(ListingDocumentType);
    address_location loc = p->m_instructions.prevHint(item->address_new);
    return loc.valid ? this->instructionItem(loc) : nullptr;
}

Symbol* ListingDocumentType::symbol(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_symboltable.get(address); }
Symbol* ListingDocumentType::symbol(const String &name) const { PIMPL_P(const ListingDocumentType); return p->m_symboltable.get(SymbolTable::normalized(name)); }
const SymbolTable *ListingDocumentType::symbols() const { PIMPL_P(const ListingDocumentType); return &p->m_symboltable; }

} // namespace REDasm
