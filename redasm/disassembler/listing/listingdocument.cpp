#include "listingdocument.h"
#include "../../support/utils.h"
#include <algorithm>
#include <sstream>

#define COMMENT_SEPARATOR " | "

namespace REDasm {

ListingDocumentType::ListingDocumentType(): sorted_container<ListingItemPtr, ListingItemPtrComparator>(), m_documententry(nullptr) { }

bool ListingDocumentType::advance(InstructionPtr &instruction)
{
    if(!instruction)
        return false;

    instruction = this->instruction(instruction->endAddress());
    return instruction && !instruction->isInvalid();
}

const ListingCursor *ListingDocumentType::cursor() const { return &m_cursor; }
ListingCursor *ListingDocumentType::cursor() { return &m_cursor; }

bool ListingDocumentType::goTo(const ListingItem *item)
{
    if(!item)
        return false;

    s64 idx = this->findIndex(item->address, item->type);

    if(idx == -1)
        return false;

    m_cursor.moveTo(idx);
    return true;
}

bool ListingDocumentType::goTo(address_t address)
{
    auto it = this->symbolItem(address);

    if(it == this->end())
        it = this->instructionItem(address);

    if(it == this->end())
        return false;

    this->goTo(it->get());
    return true;
}

void ListingDocumentType::moveToEP()
{
    if(!m_documententry)
        return;

    m_cursor.set(this->functionIndex(m_documententry->address));
}

size_t ListingDocumentType::lastLine() const { return this->size(); }
ListingFunctions &ListingDocumentType::functions() { return m_functions; }
const ListingFunctions &ListingDocumentType::functions() const { return m_functions; }
const SegmentList &ListingDocumentType::segments() const { return m_segments; }

std::string ListingDocumentType::comment(const ListingItem* item, bool skipauto) const
{
    std::string cmt;
    Detail::CommentSet comments = item->data->comments;

    if(!skipauto)
        comments.insert(item->data->autocomments.begin(), item->data->autocomments.end());

    return REDasm::join(comments, COMMENT_SEPARATOR);
}

void ListingDocumentType::comment(const ListingItem *item, const std::string &s)
{
    if(!s.empty())
        item->data->comments.insert(REDasm::simplified(s));
    else
        item->data->comments.clear();

    ListingDocumentChanged ldc(item, this->itemIndex(item));
    changed(&ldc);
}

const ListingItem *ListingDocumentType::functionStart(const ListingItem *item) const
{
    if(!item)
        return nullptr;

    size_t idx = this->itemIndex(item);

    if(idx == REDasm::npos)
        return nullptr;

    return m_functions.functionFromIndex(idx);
}

const ListingItem *ListingDocumentType::functionStart(address_t address) const
{
    size_t idx = this->instructionIndex(address);

    if(idx == REDasm::npos)
        return nullptr;

    return m_functions.functionFromIndex(idx);
}

const ListingItem *ListingDocumentType::currentFunction() const
{
    if(!this->currentItem())
        return nullptr;

    return this->functionStart(this->currentItem());
}

const ListingItem *ListingDocumentType::currentItem() const
{
    if(m_cursor.currentLine() >= this->size())
        return nullptr;

    return this->itemAt(m_cursor.currentLine());
}

Symbol* ListingDocumentType::functionStartSymbol(address_t address)
{
    const ListingItem* item = this->functionStart(address);

    if(item)
        return this->symbol(item->address);

    return nullptr;
}

InstructionPtr ListingDocumentType::entryInstruction()
{
    if(!m_documententry)
        return nullptr;

    return this->instruction(m_documententry->address);
}

const Detail::MetaItem& ListingDocumentType::meta(const ListingItem* item) const { return item->data->meta; }
std::string ListingDocumentType::type(const ListingItem* item) const { return item->data->type; }
void ListingDocumentType::empty(address_t address) { this->push(address, ListingItem::EmptyItem); }

void ListingDocumentType::meta(address_t address, const std::string &s, const std::string &name)
{
    size_t index = 0;
    auto it = m_activemeta.find(address);

    if(it != m_activemeta.end())
        index = ++it->second;
    else
        m_activemeta[address] = 0;

    ListingItem* item = this->push(address, ListingItem::MetaItem, index);

    if(name.empty())
        item->data->meta = { ".meta", s };
    else
        item->data->meta = { "." + name, s };
}

void ListingDocumentType::type(address_t address, const std::string &s)
{
    this->empty(address);
    ListingItem* item = this->push(address, ListingItem::TypeItem);
    item->data->type = s;
}

void ListingDocumentType::autoComment(address_t address, const std::string &s)
{
    if(s.empty())
        return;

    auto it = this->instructionItem(address);

    if(it == this->end())
    {
        m_pendingautocomments[address].insert(s);
        return;
    }

    it->get()->data->autocomments.insert(s);

    ListingDocumentChanged ldc(it->get(), this->itemIndex(it->get()));
    changed(&ldc);
}

void ListingDocumentType::branch(address_t address, s64 direction, tag_t tag)
{
    std::string name = REDasm::hex(address);

    if(!direction)
        name = "infinite_loop_" + name;
    else
        name = "loc_" + name;

    this->symbol(address, name, SymbolType::Code, tag);
}

void ListingDocumentType::symbol(address_t address, const std::string &name, SymbolType type, tag_t tag)
{
    Symbol* symbol = m_symboltable.symbol(address);

    if(symbol)
    {
        if(symbol->isLocked() && !(type & SymbolType::Locked))
            return;

        this->pop(address, ListingItem::EmptyItem);
        this->pop(address, ListingItem::MetaItem);

        if(symbol->isFunction())
        {
            this->push(address, ListingItem::EmptyItem);
            this->pop(address, ListingItem::FunctionItem);
        }
        else
            this->pop(address, ListingItem::SymbolItem);

        m_symboltable.erase(address);
    }

    if(!this->segment(address) || !m_symboltable.create(address, SymbolTable::normalized(name), type, tag))
        return;

    if(type & SymbolType::FunctionMask)
    {
        this->push(address, ListingItem::EmptyItem);
        this->push(address, ListingItem::FunctionItem);
    }
    else
        this->push(address, ListingItem::SymbolItem);
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
    const Symbol* symbol = m_symboltable.symbol(address);

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
        REDasm::log("Skipping empty segment " + REDasm::quoted(name));
        return;
    }

    auto it = std::find_if(m_segments.begin(), m_segments.end(), [=](const Segment& segment) -> bool {
        return segment.is(SegmentType::Bss) ? segment.contains(address) : ((segment.offset == offset) || segment.contains(address));
    });

    if(it != m_segments.end())
    {
        REDasm::log("WARNING: Segment " + REDasm::quoted(name) + " overlaps " + REDasm::quoted(it->name));
        return;
    }

    Segment segment(name, offset, address, psize, vsize, type);

    it = std::lower_bound(m_segments.begin(), m_segments.end(), segment, [](const Segment& s1, const Segment& s2) -> bool {
        return s1.address < s2.address;
    });

    m_segments.insert(it, segment);
    this->push(address, ListingItem::SegmentItem);
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
    this->lock(address, symep ? symep->name : ENTRYPOINT_FUNCTION, SymbolType::EntryPoint, tag);
    this->setDocumentEntry(address);
}

void ListingDocumentType::eraseSymbol(address_t address)
{
    this->pop(address, ListingItem::SymbolItem);
    m_symboltable.erase(address);
}

void ListingDocumentType::setDocumentEntry(address_t address)
{
    m_documententry = m_symboltable.symbol(address);
    m_cursor.set(this->functionIndex(address));
}

const Symbol *ListingDocumentType::documentEntry() const { return m_documententry; }
size_t ListingDocumentType::segmentsCount() const { return m_segments.size(); }

Segment *ListingDocumentType::segment(address_t address)
{
    for(auto it = m_segments.begin(); it != m_segments.end(); it++)
    {
        if(it->contains(address))
            return &(*it);
    }

    return nullptr;
}

const Segment *ListingDocumentType::segment(address_t address) const { return const_cast<ListingDocumentType*>(this)->segment(address); }

const Segment *ListingDocumentType::segmentByName(const std::string &name) const
{
    for(auto it = m_segments.begin(); it != m_segments.end(); it++)
    {
        const Segment& segment = *it;

        if(segment.name == name)
            return &segment;
    }

    return nullptr;
}

void ListingDocumentType::instruction(const InstructionPtr &instruction)
{
    m_instructions.commit(instruction->address, instruction);
    this->push(instruction->address, ListingItem::InstructionItem);
}

void ListingDocumentType::update(const InstructionPtr &instruction) { m_instructions.commit(instruction->address, instruction); }

InstructionPtr ListingDocumentType::instruction(address_t address)
{
    auto it = m_instructions.find(address);

    if(it != m_instructions.end())
        return *it;

    return InstructionPtr();
}

ListingDocumentType::const_iterator ListingDocumentType::functionStartItem(address_t address) const
{
    const ListingItem* item = this->functionStart(address);
    return item ? this->findItem(item) : this->end();
}

ListingDocumentType::const_iterator ListingDocumentType::functionItem(address_t address) const { return this->findItem(address, ListingItem::FunctionItem); }

ListingDocumentType::const_iterator ListingDocumentType::findItem(address_t address, size_t type, size_t index) const
{
    auto item = std::make_unique<ListingItem>(address, type, index);
    return this->find(item, ListingItemPtrFinder());
}

ListingDocumentType::const_iterator ListingDocumentType::findItem(const ListingItem *item) const { return this->findItem(item->address, item->type, item->index); }

size_t ListingDocumentType::findIndex(address_t address, size_t type, size_t index) const
{
    auto item = std::make_unique<ListingItem>(address, type, index);
    return this->indexOf(item, ListingItemPtrFinder());
}

ListingDocumentType::const_iterator ListingDocumentType::instructionItem(address_t address) const { return this->findItem(address, ListingItem::InstructionItem); }
ListingDocumentType::const_iterator ListingDocumentType::symbolItem(address_t address) const { return this->findItem(address, ListingItem::SymbolItem); }
size_t ListingDocumentType::itemIndex(const ListingItem *item) const { return this->findIndex(item->address, item->type, item->index); }
size_t ListingDocumentType::functionIndex(address_t address) const { return this->findIndex(address, ListingItem::FunctionItem); }
size_t ListingDocumentType::instructionIndex(address_t address) const { return this->findIndex(address, ListingItem::InstructionItem); }
size_t ListingDocumentType::symbolIndex(address_t address) const { return this->findIndex(address, ListingItem::SymbolItem); }

ListingItem* ListingDocumentType::itemAt(size_t i) const
{
    if(i >= this->size())
        return nullptr;

    return this->at(i).get();
}

Symbol* ListingDocumentType::symbol(address_t address) const { return m_symboltable.symbol(address); }
Symbol* ListingDocumentType::symbol(const std::string &name) const { return m_symboltable.symbol(SymbolTable::normalized(name)); }
const SymbolTable *ListingDocumentType::symbols() const { return &m_symboltable; }

ListingItem* ListingDocumentType::push(address_t address, size_t type, size_t index)
{
    auto item = std::make_unique<ListingItem>(address, type, index);

    if(type == ListingItem::InstructionItem)
    {
        auto it = m_pendingautocomments.find(address);

        if(it != m_pendingautocomments.end())
        {
            item->data->autocomments = it->second;
            m_pendingautocomments.erase(it);
        }
    }
    else if(type == ListingItem::FunctionItem)
        m_functions.insert(item.get());

    auto it = ContainerType::find(item);

    if((it != this->end()) && (((*it)->address == address) && ((*it)->type == type)))
        return it->get();

    it = ContainerType::insert(std::move(item));
    ListingDocumentChanged ldc(it->get(), std::distance(this->begin(), it), ListingDocumentChanged::Inserted);
    changed(&ldc);

    return it->get();
}

void ListingDocumentType::pop(address_t address, size_t type)
{
    ListingItemPtr item = std::make_unique<ListingItem>(address, type, 0);
    auto it = ContainerType::find(item, ListingItemPtrFinder());

    while(it != this->end())
    {
        ListingDocumentChanged ldc(it->get(), std::distance(this->begin(), it), ListingDocumentChanged::Removed);
        changed(&ldc);

        if(type == ListingItem::FunctionItem)
            m_functions.erase(it->get());

        this->erase(it);
        it = ContainerType::find(item, ListingItemPtrFinder());
    }
}

} // namespace REDasm
