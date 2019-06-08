#include "listingdocument.h"
#include "../../support/utils.h"
#include "../../support/serializer.h"
#include <impl/disassembler/listing/listingdocument_impl.h>
#include <algorithm>
#include <sstream>

#define COMMENT_SEPARATOR " | "

namespace REDasm {

ListingDocumentChanged::ListingDocumentChanged(const ListingItem *item, size_t index, ListingDocumentAction action): m_pimpl_p(new ListingDocumentChangedImpl(item, index, action)) {  }
const ListingItem *ListingDocumentChanged::item() const { PIMPL_P(const ListingDocumentChanged); return p->item(); }
ListingDocumentAction ListingDocumentChanged::action() const { PIMPL_P(const ListingDocumentChanged); return p->action(); }
bool ListingDocumentChanged::isInserted() const { PIMPL_P(const ListingDocumentChanged); return p->isInserted(); }
bool ListingDocumentChanged::isRemoved() const { PIMPL_P(const ListingDocumentChanged); return p->isRemoved(); }
size_t ListingDocumentChanged::index() const { PIMPL_P(const ListingDocumentChanged); return p->index(); }

ListingDocumentType::ListingDocumentType(): m_pimpl_p(new ListingDocumentTypeImpl(this)) { }
size_t ListingDocumentType::size() const { PIMPL_P(const ListingDocumentType); return p->size(); }
bool ListingDocumentType::empty() const { PIMPL_P(const ListingDocumentType); return p->empty(); }

bool ListingDocumentType::advance(InstructionPtr &instruction)
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
    size_t idx = p->findIndex(item->address(), item->type());

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
const ListingFunctions* ListingDocumentType::functions() const { PIMPL_P(const ListingDocumentType); return &p->m_functions; }
const SegmentList &ListingDocumentType::segments() const { PIMPL_P(const ListingDocumentType); return p->m_segments; }

std::string ListingDocumentType::comment(const ListingItem* item, bool skipauto) const
{
    std::string cmt;
    ListingCommentSet comments = item->data()->comments;

    if(!skipauto)
        comments.insert(item->data()->autocomments.begin(), item->data()->autocomments.end());

    return Utils::join(comments, COMMENT_SEPARATOR);
}

void ListingDocumentType::comment(const ListingItem *item, const std::string &s)
{
    if(!s.empty())
        item->data()->comments.insert(Utils::simplified(s));
    else
        item->data()->comments.clear();

    ListingDocumentChanged ldc(item, this->itemIndex(item));
    changed(&ldc);
}

const ListingItem *ListingDocumentType::functionStart(const ListingItem *item) const
{
    if(!item)
        return nullptr;

    if(item->is(ListingItemType::FunctionItem))
        return item;

    size_t idx = this->itemIndex(item);

    if(idx == REDasm::npos)
        return nullptr;

    PIMPL_P(const ListingDocumentType);
    return p->m_functions.functionFromIndex(idx);
}

const ListingItem *ListingDocumentType::functionStart(address_t address) const
{
    size_t idx = this->instructionIndex(address);

    if(idx == REDasm::npos)
        return nullptr;

    PIMPL_P(const ListingDocumentType);
    return p->m_functions.functionFromIndex(idx);
}

const ListingItem *ListingDocumentType::currentFunction() const
{
    if(!this->currentItem())
        return nullptr;

    return this->functionStart(this->currentItem());
}

const ListingItem *ListingDocumentType::currentItem() const
{
    PIMPL_P(const ListingDocumentType);

    if(p->m_cursor.currentLine() >= this->size())
        return nullptr;

    return this->itemAt(p->m_cursor.currentLine());
}

const ListingItem *ListingDocumentType::segmentItem(address_t address) const
{
    PIMPL_P(const ListingDocumentType);
    auto it = p->segmentIterator(address);

    if(it == p->end())
        return nullptr;

    return it->get();
}

const ListingItem *ListingDocumentType::symbolItem(address_t address) const
{
    PIMPL_P(const ListingDocumentType);
    auto it = p->symbolIterator(address);

    if(it == p->end())
        return nullptr;

    return it->get();
}

const ListingItem *ListingDocumentType::instructionItem(address_t address) const
{
    PIMPL_P(const ListingDocumentType);
    auto it = p->instructionIterator(address);

    if(it == p->end())
        return nullptr;

    return it->get();
}

const ListingItem *ListingDocumentType::functionItem(address_t address) const
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
        return this->symbol(item->address());

    return nullptr;
}

InstructionPtr ListingDocumentType::entryInstruction()
{
    PIMPL_P(const ListingDocumentType);

    if(!p->m_documententry)
        return nullptr;

    return this->instruction(p->m_documententry->address);
}

const ListingMetaItem &ListingDocumentType::meta(const ListingItem* item) const { return item->data()->meta; }
std::string ListingDocumentType::type(const ListingItem* item) const { return item->data()->type; }
void ListingDocumentType::empty(address_t address) { PIMPL_P(ListingDocumentType); p->push(address, ListingItemType::EmptyItem); }

void ListingDocumentType::meta(address_t address, const std::string &s, const std::string &name)
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

void ListingDocumentType::type(address_t address, const std::string &s)
{
    PIMPL_P(ListingDocumentType);

    this->empty(address);
    ListingItem* item = p->push(address, ListingItemType::TypeItem);
    item->data()->type = s;
}

void ListingDocumentType::autoComment(address_t address, const std::string &s)
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

    ListingDocumentChanged ldc(it->get(), this->itemIndex(it->get()));
    changed(&ldc);
}

void ListingDocumentType::branch(address_t address, s64 direction, tag_t tag)
{
    std::string name = Utils::hex(address);

    if(!direction)
        name = "infinite_loop_" + name;
    else
        name = "loc_" + name;

    this->symbol(address, name, SymbolType::Code, tag);
}

void ListingDocumentType::symbol(address_t address, const std::string &name, SymbolType type, tag_t tag)
{
    PIMPL_P(ListingDocumentType);
    Symbol* symbol = p->m_symboltable.symbol(address);

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

    if(!this->segment(address) || !p->m_symboltable.create(address, SymbolTable::normalized(name), type, tag))
        return;

    if(type & SymbolType::FunctionMask)
    {
        p->push(address, ListingItemType::EmptyItem);
        p->push(address, ListingItemType::FunctionItem);
    }
    else
        p->push(address, ListingItemType::SymbolItem);
}

void ListingDocumentType::symbol(address_t address, SymbolType type, tag_t tag) { this->symbol(address, SymbolTable::name(address, type), type, tag); }

void ListingDocumentType::rename(address_t address, const std::string &name)
{
    if(name.empty())
        return;

    const Symbol* symbol = this->symbol(address);

    if(!symbol)
        return;

    this->symbol(address, name, symbol->type, symbol->tag);
}

void ListingDocumentType::lock(address_t address, const std::string &name)
{
    PIMPL_P(ListingDocumentType);
    const Symbol* symbol = p->m_symboltable.symbol(address);

    if(!symbol)
        this->lock(address, name.empty() ? symbol->name : name, SymbolType::Data);
    else
        this->lock(address, name.empty() ? symbol->name : name, symbol->type, symbol->tag);
}

void ListingDocumentType::lock(address_t address, SymbolType type, tag_t tag) { this->symbol(address, type | SymbolType::Locked, tag); }
void ListingDocumentType::lock(address_t address, const std::string &name, SymbolType type, tag_t tag) { this->symbol(address, name, type | SymbolType::Locked, tag); }
void ListingDocumentType::segment(const std::string &name, offset_t offset, address_t address, u64 size, SegmentType type) { this->segment(name, offset, address, size, size, type); }

void ListingDocumentType::segment(const std::string &name, offset_t offset, address_t address, u64 psize, u64 vsize, SegmentType type)
{
    if(!psize && !vsize)
    {
        r_ctx->log("Skipping empty segment " + Utils::quoted(name));
        return;
    }

    PIMPL_P(ListingDocumentType);

    auto it = std::find_if(p->m_segments.begin(), p->m_segments.end(), [=](const Segment& segment) -> bool {
        return segment.is(SegmentType::Bss) ? segment.contains(address) : ((segment.offset == offset) || segment.contains(address));
    });

    if(it != p->m_segments.end())
    {
        r_ctx->problem("Segment " + Utils::quoted(name) + " overlaps " + Utils::quoted(it->name));
        return;
    }

    Segment segment(name, offset, address, psize, vsize, type);

    it = std::lower_bound(p->m_segments.begin(), p->m_segments.end(), segment, [](const Segment& s1, const Segment& s2) -> bool {
        return s1.address < s2.address;
    });

    p->m_segments.insert(it, segment);
    p->push(address, ListingItemType::SegmentItem);
}

void ListingDocumentType::lockFunction(address_t address, const std::string &name, u32 tag) { this->lock(address, name, SymbolType::Function, tag);  }
void ListingDocumentType::function(address_t address, const std::string &name, tag_t tag) { this->symbol(address, name, SymbolType::Function, tag); }
void ListingDocumentType::function(address_t address, tag_t tag) { this->symbol(address, SymbolType::Function, tag); }
void ListingDocumentType::pointer(address_t address, SymbolType type, tag_t tag) { this->symbol(address, type | SymbolType::Pointer, tag); }

void ListingDocumentType::table(address_t address, u64 count, tag_t tag)
{
    this->lock(address, SymbolTable::name(address, SymbolType::TableItem) + "_0", SymbolType::TableItem, tag);
    this->type(address, "Table with " + std::to_string(count) + " case(s)");
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
               SymbolTable::name(startaddress, SymbolType::TableItem) + "_" + std::to_string(idx),
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
    p->m_documententry = p->m_symboltable.symbol(address);
    p->m_cursor.set(this->functionIndex(address));
}

const Symbol *ListingDocumentType::documentEntry() const { PIMPL_P(const ListingDocumentType); return p->m_documententry; }
size_t ListingDocumentType::segmentsCount() const { PIMPL_P(const ListingDocumentType); return p->m_segments.size(); }

Segment *ListingDocumentType::segment(address_t address)
{
    PIMPL_P(ListingDocumentType);

    for(auto it = p->m_segments.begin(); it != p->m_segments.end(); it++)
    {
        if(it->contains(address))
            return &(*it);
    }

    return nullptr;
}

const Segment *ListingDocumentType::segment(address_t address) const { return const_cast<ListingDocumentType*>(this)->segment(address); }

const Segment *ListingDocumentType::segmentByName(const std::string &name) const
{
    PIMPL_P(const ListingDocumentType);

    for(auto it = p->m_segments.begin(); it != p->m_segments.end(); it++)
    {
        const Segment& segment = *it;

        if(segment.name == name)
            return &segment;
    }

    return nullptr;
}

void ListingDocumentType::instruction(const InstructionPtr &instruction)
{
    PIMPL_P(ListingDocumentType);
    p->m_instructions.commit(instruction->address, instruction);
    p->push(instruction->address, ListingItemType::InstructionItem);
}

void ListingDocumentType::update(const InstructionPtr &instruction) { PIMPL_P(ListingDocumentType); p->m_instructions.commit(instruction->address, instruction); }

InstructionPtr ListingDocumentType::instruction(address_t address)
{
    PIMPL_P(ListingDocumentType);
    auto it = p->m_instructions.find(address);

    if(it != p->m_instructions.end())
        return *it;

    return InstructionPtr();
}

size_t ListingDocumentType::itemIndex(const ListingItem *item) const { PIMPL_P(const ListingDocumentType); return p->findIndex(item->address(), item->type(), item->index()); }
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

Symbol* ListingDocumentType::symbol(address_t address) const { PIMPL_P(const ListingDocumentType); return p->m_symboltable.symbol(address); }
Symbol* ListingDocumentType::symbol(const std::string &name) const { PIMPL_P(const ListingDocumentType); return p->m_symboltable.symbol(SymbolTable::normalized(name)); }
const SymbolTable *ListingDocumentType::symbols() const { PIMPL_P(const ListingDocumentType); return &p->m_symboltable; }

void Serializer<ListingDocument>::write(std::fstream& fs, const ListingDocument& d) {
    auto lock = x_lock_safe_ptr(d);

    Serializer<SegmentList>::write(fs, lock->pimpl_p()->m_segments);
    Serializer<SymbolTable>::write(fs, &lock->pimpl_p()->m_symboltable);

    Serializer<typename ListingDocumentTypeImpl::ContainerType>::write(fs, *lock.t->pimpl_p());

    Serializer<address_t>::write(fs, (lock->pimpl_p()->m_documententry ? lock->pimpl_p()->m_documententry->address : 0));
    Serializer<ListingCursor>::write(fs, &lock->pimpl_p()->m_cursor);
}

void Serializer<ListingDocument>::read(std::fstream& fs, ListingDocument& d, const std::function<InstructionPtr(address_t address)> cb) {
    auto lock = x_lock_safe_ptr(d);

    Serializer<SegmentList>::read(fs, lock->pimpl_p()->m_segments);
    Serializer<SymbolTable>::read(fs, &lock->pimpl_p()->m_symboltable);

    Serializer<typename ListingDocumentTypeImpl::ContainerType>::read(fs, [&](ListingItemPtr item) {
        if(item->is(ListingItemType::InstructionItem))
            lock->pimpl_p()->m_instructions.commit(item->address(), cb(item->address()));

        lock->pimpl_p()->insert(std::move(item));
    });

    address_t entry = 0;
    Serializer<address_t>::read(fs, entry);
    lock->pimpl_p()->m_documententry = lock->symbol(entry);

    Serializer<ListingCursor>::read(fs, &lock->pimpl_p()->m_cursor);
}

} // namespace REDasm
