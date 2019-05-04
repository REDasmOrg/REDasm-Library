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

void ListingDocumentType::serializeTo(std::fstream &fs)
{
    //Serializer::serializeScalar(fs, m_cursor.currentLine());
    //Serializer::serializeScalar(fs, m_cursor.currentColumn());
    //Serializer::serializeScalar(fs, m_documententry ? m_documententry->address : 0);

    //// Segments
    //Serializer::serializeArray<std::deque, Segment>(fs, m_segments, [&](const Segment& s) {
    //    Serializer::serializeString(fs, s.name);
    //    Serializer::serializeScalar(fs, s.offset);
    //    Serializer::serializeScalar(fs, s.endoffset);
    //    Serializer::serializeScalar(fs, s.address);
    //    Serializer::serializeScalar(fs, s.endaddress);
    //    Serializer::serializeScalar(fs, s.type);
    //});

    //// Auto Comments
    //Serializer::serializeMap<address_t, CommentSet>(fs, m_autocomments, [&](address_t k, const CommentSet& v) {
    //    Serializer::serializeScalar(fs, k);

    //    Serializer::serializeArray<std::set, std::string>(fs, v, [&](const std::string& s) {
    //        Serializer::serializeString(fs, s);
    //    });
    //});

    //// User Comments
    //Serializer::serializeMap<address_t, std::string>(fs, m_comments, [&](address_t k, const std::string& v) {
    //    Serializer::serializeScalar(fs, k);
    //    Serializer::serializeString(fs, v);
    //});

    //// Metadata
    //Serializer::serializeMap<address_t, MetaList>(fs, m_meta, [&](address_t k, const MetaList& v) {
    //    Serializer::serializeScalar(fs, k);

    //    Serializer::serializeArray<std::deque, std::pair<std::string, std::string> >(fs, v, [&](const std::pair<std::string, std::string>& m) {
    //        Serializer::serializeString(fs, m.first);
    //        Serializer::serializeString(fs, m.second);
    //    });
    //});

    //m_instructions.serializeTo(fs);
    //m_symboltable.serializeTo(fs);
}

void ListingDocumentType::deserializeFrom(std::fstream &fs)
{
    //address_t ep = 0;
    //u64 line = 0, column = 0;
    //Serializer::deserializeScalar(fs, &line);
    //Serializer::deserializeScalar(fs, &column);
    //Serializer::deserializeScalar(fs, &ep);

    //// Segments
    //Serializer::deserializeArray<std::deque, Segment>(fs, m_segments, [&](Segment& s) {
    //    Serializer::deserializeString(fs, s.name);
    //    Serializer::deserializeScalar(fs, &s.offset);
    //    Serializer::deserializeScalar(fs, &s.endoffset);
    //    Serializer::deserializeScalar(fs, &s.address);
    //    Serializer::deserializeScalar(fs, &s.endaddress);
    //    Serializer::deserializeScalar(fs, &s.type);

    //    this->push(s.address, ListingItem::SegmentItem);
    //});

    //// Auto Comments
    //Serializer::deserializeMap<address_t, CommentSet>(fs, m_autocomments, [&](address_t& k, CommentSet& v) {
    //    Serializer::deserializeScalar(fs, &k);

    //    Serializer::deserializeArray<std::set, std::string>(fs, v, [&](std::string& s) {
    //        Serializer::deserializeString(fs, s);
    //    });
    //});

    //// User Comments
    //Serializer::deserializeMap<address_t, std::string>(fs, m_comments, [&](address_t& k, std::string& v) {
    //    Serializer::deserializeScalar(fs, &k);
    //    Serializer::deserializeString(fs, v);
    //});

    //// Metadata
    //Serializer::deserializeMap<address_t, MetaList>(fs, m_meta, [&](address_t& k, MetaList& v) {
    //    Serializer::deserializeScalar(fs, &k);
    //    size_t idx = 0;

    //    Serializer::deserializeArray<std::deque, std::pair<std::string, std::string> >(fs, v, [&](std::pair<std::string, std::string>& m) {
    //        Serializer::deserializeString(fs, m.first);
    //        Serializer::deserializeString(fs, m.second);

    //        if(!idx)
    //            this->push(k, ListingItem::EmptyItem);

    //        this->push(k, ListingItem::MetaItem, idx);
    //        idx++;
    //    });
    //});

    //EVENT_CONNECT(&m_instructions, deserialized, this, [&](const InstructionPtr& instruction) {
    //    this->push(instruction->address, ListingItem::InstructionItem);
    //});

    //EVENT_CONNECT(&m_symboltable, deserialized, this, [&](const Symbol* symbol) {
    //    if(symbol->type & SymbolTypes::FunctionMask) {
    //        this->push(symbol->address, ListingItem::EmptyItem);
    //        this->push(symbol->address, ListingItem::FunctionItem);
    //    }
    //    else
    //        this->push(symbol->address, ListingItem::SymbolItem);
    //});

    //m_instructions.deserializeFrom(fs);
    //m_symboltable.deserializeFrom(fs);

    //m_instructions.deserialized.removeLast();
    //m_symboltable.deserialized.removeLast();

    //m_documententry = m_symboltable.symbol(ep);
    //m_cursor.set(line, column);
}

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

void ListingDocumentType::branch(address_t address, s64 direction, u32 tag)
{
    std::string name = REDasm::hex(address);

    if(!direction)
        name = "infinite_loop_" + name;
    else
        name = "loc_" + name;

    this->symbol(address, name, SymbolTypes::Code, tag);
}

void ListingDocumentType::symbol(address_t address, const std::string &name, u32 type, u32 tag)
{
    Symbol* symbol = m_symboltable.symbol(address);

    if(symbol)
    {
        if(symbol->isLocked() && !(type & SymbolTypes::Locked))
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

    if(type & SymbolTypes::FunctionMask)
    {
        this->push(address, ListingItem::EmptyItem);
        this->push(address, ListingItem::FunctionItem);
    }
    else
        this->push(address, ListingItem::SymbolItem);
}

void ListingDocumentType::symbol(address_t address, u32 type, u32 tag) { this->symbol(address, SymbolTable::name(address, type), type, tag); }

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
        this->lock(address, name.empty() ? symbol->name : name, SymbolTypes::Data);
    else
        this->lock(address, name.empty() ? symbol->name : name, symbol->type, symbol->tag);
}

void ListingDocumentType::lock(address_t address, u32 type, u32 tag) { this->symbol(address, type | SymbolTypes::Locked, tag); }
void ListingDocumentType::lock(address_t address, const std::string &name, u32 type, u32 tag) { this->symbol(address, name, type | SymbolTypes::Locked, tag); }
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

void ListingDocumentType::lockFunction(address_t address, const std::string &name, u32 tag) { this->lock(address, name, SymbolTypes::Function, tag);  }
void ListingDocumentType::function(address_t address, const std::string &name, u32 tag) { this->symbol(address, name, SymbolTypes::Function, tag); }
void ListingDocumentType::function(address_t address, u32 tag) { this->symbol(address, SymbolTypes::Function, tag); }
void ListingDocumentType::pointer(address_t address, u32 type, u32 tag) { this->symbol(address, type | SymbolTypes::Pointer, tag); }

void ListingDocumentType::table(address_t address, u64 count, u32 tag)
{
    this->lock(address, SymbolTable::name(address, SymbolTypes::TableItem) + "_0", SymbolTypes::TableItem, tag);
    this->type(address, "Table with " + std::to_string(count) + " case(s)");
}

void ListingDocumentType::tableItem(address_t address, address_t startaddress, u64 idx, u32 tag)
{
    Symbol* symbol = this->symbol(address); // Don't override custom symbols, if any

    if(symbol)
    {
        symbol->type |= SymbolTypes::TableItem;
        this->lock(address, symbol->name, symbol->type, tag);
        return;
    }

    this->lock(address,
               SymbolTable::name(startaddress, SymbolTypes::TableItem) + "_" + std::to_string(idx),
               SymbolTypes::TableItem, tag);
}

void ListingDocumentType::entry(address_t address, u32 tag)
{
    const Symbol* symep = this->symbol(address); // Don't override custom symbols, if any
    this->lock(address, symep ? symep->name : ENTRYPOINT_FUNCTION, SymbolTypes::EntryPoint, tag);
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

void ListingDocumentType::update(const InstructionPtr &instruction) { m_instructions.update(instruction); }

InstructionPtr ListingDocumentType::instruction(address_t address)
{
    auto it = m_instructions.find(address);

    if(it != m_instructions.end())
        return *it;

    return InstructionPtr();
}

ListingDocumentType::const_iterator ListingDocumentType::functionItem(address_t address) const { return this->findItem(address, ListingItem::FunctionItem); }

ListingDocumentType::const_iterator ListingDocumentType::findItem(address_t address, size_t type, size_t index) const
{
    auto item = std::make_unique<ListingItem>(address, type, index);
    return this->find(item, ListingItemPtrFinder());
}

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
