#include "listingdocument.h"
#include "../../support/utils.h"
#include <redasm/support/demangler.h>
#include <algorithm>
#include <sstream>

#define COMMENT_SEPARATOR " | "

namespace REDasm {

ListingDocumentType::ListingDocumentType(): std::deque<ListingItemPtr>() { }

bool ListingDocumentType::advance(InstructionPtr &instruction)
{
    if(!instruction)
        return false;

    instruction = this->instruction(instruction->endAddress());
    return instruction && !instruction->isInvalid();
}

const ListingCursor *ListingDocumentType::cursor() const { return &m_cursor; }
ListingCursor *ListingDocumentType::cursor() { return &m_cursor; }

void ListingDocumentType::moveToEP()
{
    if(!m_documententry)
        return;

    m_cursor.set(this->functionIndex(m_documententry->address));
}

u64 ListingDocumentType::lastLine() const { return static_cast<u64>(this->size()) - 1; }

void ListingDocumentType::serializeTo(std::fstream &fs)
{
    Serializer::serializeScalar(fs, m_cursor.currentLine());
    Serializer::serializeScalar(fs, m_cursor.currentColumn());
    Serializer::serializeScalar(fs, m_documententry ? m_documententry->address : 0);

    // Segments
    Serializer::serializeArray<std::vector, Segment>(fs, m_segments, [&](const Segment& s) {
        Serializer::serializeString(fs, s.name);
        Serializer::serializeScalar(fs, s.offset);
        Serializer::serializeScalar(fs, s.endoffset);
        Serializer::serializeScalar(fs, s.address);
        Serializer::serializeScalar(fs, s.endaddress);
        Serializer::serializeScalar(fs, s.type);
    });

    // Auto Comments
    Serializer::serializeMap<address_t, CommentSet>(fs, m_autocomments, [&](const AutoCommentItem& aci) {
        Serializer::serializeScalar(fs, aci.first);

        Serializer::serializeArray<std::set, std::string>(fs, aci.second, [&](const std::string& s) {
            Serializer::serializeString(fs, s);
        });
    });

    // User Comments
    Serializer::serializeMap<address_t, std::string>(fs, m_comments, [&](const CommentItem& ci) {
        Serializer::serializeScalar(fs, ci.first);
        Serializer::serializeString(fs, ci.second);
    });

    m_instructions.serializeTo(fs);
    m_symboltable.serializeTo(fs);
}

void ListingDocumentType::deserializeFrom(std::fstream &fs)
{
    address_t ep = 0;
    u64 line = 0, column = 0;
    Serializer::deserializeScalar(fs, &line);
    Serializer::deserializeScalar(fs, &column);
    Serializer::deserializeScalar(fs, &ep);

    // Segments
    Serializer::deserializeArray<std::vector, Segment>(fs, m_segments, [&](Segment& s) {
        Serializer::deserializeString(fs, s.name);
        Serializer::deserializeScalar(fs, &s.offset);
        Serializer::deserializeScalar(fs, &s.endoffset);
        Serializer::deserializeScalar(fs, &s.address);
        Serializer::deserializeScalar(fs, &s.endaddress);
        Serializer::deserializeScalar(fs, &s.type);

        this->insertSorted(s.address, ListingItem::SegmentItem);
    });

    // Auto Comments
    Serializer::deserializeMap<address_t, CommentSet>(fs, m_autocomments, [&](AutoCommentItem& ci) {
        Serializer::deserializeScalar(fs, &ci.first);

        Serializer::deserializeArray<std::set, std::string>(fs, ci.second, [&](std::string& s) {
            Serializer::deserializeString(fs, s);
        });
    });

    // User Comments
    Serializer::deserializeMap<address_t, std::string>(fs, m_comments, [&](CommentItem& ci) {
        Serializer::deserializeScalar(fs, &ci.first);
        Serializer::deserializeString(fs, ci.second);
    });

    m_instructions.deserialized += [&](const InstructionPtr& instruction) {
        this->insertSorted(instruction->address, ListingItem::InstructionItem);
    };

    m_symboltable.deserialized += [&](const SymbolPtr& symbol) {
        if(symbol->type & SymbolTypes::FunctionMask)
            this->insertSorted(symbol->address, ListingItem::FunctionItem);
        else
            this->insertSorted(symbol->address, ListingItem::SymbolItem);
    };

    m_instructions.deserializeFrom(fs);
    m_symboltable.deserializeFrom(fs);

    m_instructions.deserialized.removeLast();
    m_symboltable.deserialized.removeLast();

    m_documententry = m_symboltable.symbol(ep);
    m_cursor.set(line, column);
}

ListingItems ListingDocumentType::getCalls(ListingItem *item)
{
    ListingItems calls;
    ListingDocumentType::iterator it = this->end();

    if(item->is(ListingItem::InstructionItem))
    {
        InstructionPtr instruction = this->instruction(item->address);

        if(!instruction->hasTargets())
            return ListingItems();

        it = this->instructionItem(instruction->target());
    }
    else
        it = this->instructionItem(item->address);

    for( ; it != this->end(); it++)
    {
        ListingItem* item = it->get();

        if(item->is(ListingItem::InstructionItem))
        {
            InstructionPtr instruction = this->instruction(item->address);

            if(!instruction->is(InstructionTypes::Call))
                continue;

            calls.push_back(item);
        }
        else if(item->is(ListingItem::SymbolItem))
        {
            SymbolPtr symbol = this->symbol(item->address);

            if(!symbol->is(SymbolTypes::Code))
                break;
        }
        else
            break;
    }

    return calls;
}

ListingItem *ListingDocumentType::functionStart(ListingItem *item)
{
    if(!item)
        return nullptr;

    return this->functionStart(item->address);
}

ListingItem *ListingDocumentType::functionStart(address_t address)
{
    auto iit = this->instructionItem(address);

    if(iit == this->end())
        return nullptr;

    auto fit = std::upper_bound(m_functions.begin(), m_functions.end(), iit->get(), [](const ListingItem* item1, const ListingItem* item2) {
        return item1->address < item2->address;
    });

    if(fit == m_functions.end())
        return m_functions.back();

    if((*fit)->address > iit->get()->address)
    {
        if(fit == m_functions.begin()) // Function not found
            return nullptr;

        fit--;
    }

    return *fit;
}

ListingItem *ListingDocumentType::currentItem()
{
    if(m_cursor.currentLine() >= static_cast<u64>(this->size()))
        return nullptr;

    return this->itemAt(m_cursor.currentLine());
}

SymbolPtr ListingDocumentType::functionStartSymbol(address_t address)
{
    ListingItem* item = this->functionStart(address);

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

std::string ListingDocumentType::comment(address_t address, bool skipauto) const
{
    std::string cmt;
    auto it = m_comments.find(address);

    if(it != m_comments.end())
        cmt = it->second;

    if(skipauto)
        return cmt;

    std::string acmt = this->autoComment(address);

    if(!acmt.empty())
        return cmt.empty() ? acmt : (cmt + COMMENT_SEPARATOR + acmt);

    return cmt;
}

std::string ListingDocumentType::info(address_t address) const
{
    auto it = m_info.find(address);

    if(it != m_info.end())
        return it->second;

    return std::string();
}

std::string ListingDocumentType::type(address_t address) const
{
    auto it = m_types.find(address);

    if(it != m_types.end())
        return it->second;

    return std::string();
}

void ListingDocumentType::empty(address_t address) { this->insertSorted(address, ListingItem::EmptyItem); }

void ListingDocumentType::info(address_t address, const std::string &s)
{
    m_info[address] = s;

    this->empty(address);
    this->insertSorted(address, ListingItem::InfoItem);
}

void ListingDocumentType::type(address_t address, const std::string &s)
{
    m_types[address] = s;

    this->empty(address);
    this->insertSorted(address, ListingItem::TypeItem);
}

void ListingDocumentType::comment(address_t address, const std::string &s)
{
    if(!s.empty())
        m_comments[address] = REDasm::simplified(s);
    else
        m_comments.erase(address);

    auto iit = this->instructionItem(address);

    if(iit == this->end())
        return;

    ListingDocumentChanged ldc(iit->get(), std::distance(this->begin(), iit));
    changed(&ldc);
}

void ListingDocumentType::autoComment(address_t address, const std::string &s)
{
    if(s.empty())
        return;

    auto it = m_autocomments.find(address);

    if(it == m_autocomments.end())
    {
        CommentSet cs;
        cs.insert(s);
        m_autocomments[address] = cs;
    }
    else
        it->second.insert(s);

    auto iit = this->instructionItem(address);

    if(iit == this->end())
        return;

    ListingDocumentChanged ldc(iit->get(), std::distance(this->begin(), iit));
    changed(&ldc);
}

void ListingDocumentType::symbol(address_t address, const std::string &name, u32 type, u32 tag)
{
    SymbolPtr symbol = m_symboltable.symbol(address);

    if(symbol)
    {
        if(symbol->isLocked() && !(type & SymbolTypes::Locked))
            return;

        this->removeSorted(address, ListingItem::EmptyItem);
        this->removeSorted(address, ListingItem::InfoItem);

        if(symbol->isFunction())
            this->removeSorted(address, ListingItem::FunctionItem);
        else
            this->removeSorted(address, ListingItem::SymbolItem);

        m_symboltable.erase(address);
    }

    if(!this->segment(address) || !m_symboltable.create(address, ListingDocumentType::normalized(name), type, tag))
        return;

    if(type & SymbolTypes::FunctionMask)
    {
        if(Demangler::isMangled(name))
            this->info(address, Demangler::demangled(name));
        else
            this->insertSorted(address, ListingItem::EmptyItem);

        this->insertSorted(address, ListingItem::FunctionItem);
    }
    else
        this->insertSorted(address, ListingItem::SymbolItem);
}

void ListingDocumentType::symbol(address_t address, u32 type, u32 tag)
{
    if(type & SymbolTypes::TableMask)
        this->symbol(address, ListingDocumentType::symbolName("tbl", address), type, tag);
    else if(type & SymbolTypes::TableItem)
        this->symbol(address, ListingDocumentType::symbolName("tbi", address), type, tag);
    else if(type & SymbolTypes::Pointer)
        this->symbol(address, ListingDocumentType::symbolName("ptr", address), type, tag);
    else if(type & SymbolTypes::WideStringMask)
        this->symbol(address, ListingDocumentType::symbolName("wstr", address), type, tag);
    else if(type & SymbolTypes::StringMask)
        this->symbol(address, ListingDocumentType::symbolName("str", address), type, tag);
    else if(type & SymbolTypes::FunctionMask)
        this->symbol(address, ListingDocumentType::symbolName("sub", address), type, tag);
    else
    {
        const Segment* segment = this->segment(address);

        if(segment && segment->is(SegmentTypes::Code))
            this->symbol(address, ListingDocumentType::symbolName("loc", address), type, tag);
        else
            this->symbol(address, ListingDocumentType::symbolName("data", address), type, tag);
    }
}

void ListingDocumentType::rename(address_t address, const std::string &name)
{
    if(name.empty())
        return;

    SymbolPtr symbol = this->symbol(address);

    if(!symbol)
        return;

    this->symbol(address, name, symbol->type, symbol->tag);
}

void ListingDocumentType::lock(address_t address, const std::string &name)
{
    SymbolPtr symbol = m_symboltable.symbol(address);

    if(!symbol)
        this->lock(address, name.empty() ? symbol->name : name, SymbolTypes::Data);
    else
        this->lock(address, name.empty() ? symbol->name : name, symbol->type, symbol->tag);
}

void ListingDocumentType::lock(address_t address, u32 type, u32 tag) { this->symbol(address, type | SymbolTypes::Locked, tag); }
void ListingDocumentType::lock(address_t address, const std::string &name, u32 type, u32 tag) { this->symbol(address, name, type | SymbolTypes::Locked, tag); }
void ListingDocumentType::segment(const std::string &name, offset_t offset, address_t address, u64 size, u32 type) { this->segment(name, offset, address, size, size, type); }

void ListingDocumentType::segment(const std::string &name, offset_t offset, address_t address, u64 psize, u32 vsize, u32 type)
{
    if(!psize)
    {
        REDasm::log("Skipping empty segment " + REDasm::quoted(name));
        return;
    }

    if(!vsize)
        vsize = psize;

    auto it = std::find_if(m_segments.begin(), m_segments.end(), [=](const Segment& segment) -> bool {
        if(segment.is(SegmentTypes::Bss))
            return segment.contains(address);

        return (segment.offset == offset) || segment.contains(address);
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

    it = m_segments.insert(it, segment);
    this->insertSorted(address, ListingItem::SegmentItem);
}

void ListingDocumentType::lockFunction(address_t address, const std::string &name, u32 tag) { this->lock(address, name, SymbolTypes::Function, tag);  }
void ListingDocumentType::function(address_t address, const std::string &name, u32 tag) { this->symbol(address, name, SymbolTypes::Function, tag); }
void ListingDocumentType::function(address_t address, u32 tag) { this->symbol(address, SymbolTypes::Function, tag); }
void ListingDocumentType::pointer(address_t address, u32 type, u32 tag) { this->symbol(address, type | SymbolTypes::Pointer, tag); }
void ListingDocumentType::table(address_t address, u32 tag) { this->lock(address, SymbolTypes::Table, tag); }

void ListingDocumentType::tableItem(address_t address, u32 type, u32 tag)
{
    type |= SymbolTypes::TableItem;
    SymbolPtr symbol = this->symbol(address); // Don't override custom symbols, if any

    if(symbol)
    {
        symbol->type |= type;
        this->lock(address, symbol->name, symbol->type, tag);
        return;
    }

    this->lock(address, type, tag);
}

void ListingDocumentType::entry(address_t address, u32 tag)
{
    SymbolPtr symep = this->symbol(address); // Don't override custom symbols, if any
    this->lock(address, symep ? symep->name : ENTRYPOINT_FUNCTION, SymbolTypes::EntryPoint, tag);
    this->setDocumentEntry(address);
}

void ListingDocumentType::eraseSymbol(address_t address)
{
    this->removeSorted(address, ListingItem::SymbolItem);
    m_symboltable.erase(address);
}

void ListingDocumentType::setDocumentEntry(address_t address)
{
    m_documententry = m_symboltable.symbol(address);
    m_cursor.set(this->functionIndex(address));
}

SymbolPtr ListingDocumentType::documentEntry() const { return m_documententry; }
size_t ListingDocumentType::segmentsCount() const { return m_segments.size(); }
size_t ListingDocumentType::functionsCount() const { return m_functions.size(); }

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
const Segment *ListingDocumentType::segmentAt(size_t idx) const { return &m_segments[idx]; }

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
    this->insertSorted(instruction->address, ListingItem::InstructionItem);
}

void ListingDocumentType::update(const InstructionPtr &instruction) { m_instructions.update(instruction); }

InstructionPtr ListingDocumentType::instruction(address_t address)
{
    auto it = m_instructions.find(address);

    if(it != m_instructions.end())
        return *it;

    return InstructionPtr();
}

ListingDocumentType::iterator ListingDocumentType::functionItem(address_t address) { return this->item(address, ListingItem::FunctionItem); }
ListingDocumentType::iterator ListingDocumentType::item(address_t address, u32 type) { return Listing::binarySearch(this, address, type); }
int ListingDocumentType::index(address_t address, u32 type) { return Listing::indexOf(this, address, type); }

std::string ListingDocumentType::autoComment(address_t address) const
{
    auto it = m_autocomments.find(address);

    if(it == m_autocomments.end())
        return std::string();

    std::string cmt;

    for(const std::string& s : it->second)
    {
        if(!cmt.empty())
            cmt += COMMENT_SEPARATOR;

        cmt += s;
    }

    return cmt;
}

std::string ListingDocumentType::normalized(std::string s)
{
    std::replace(s.begin(), s.end(), ' ', '_');
    return s;
}

ListingDocumentType::iterator ListingDocumentType::instructionItem(address_t address) { return this->item(address, ListingItem::InstructionItem); }
ListingDocumentType::iterator ListingDocumentType::symbolItem(address_t address) { return this->item(address, ListingItem::SymbolItem); }

ListingDocumentType::iterator ListingDocumentType::item(address_t address)
{
    auto it = this->symbolItem(address);

    if(it == this->end())
        it = this->instructionItem(address);

    return it;
}

int ListingDocumentType::functionIndex(address_t address) { return this->index(address, ListingItem::FunctionItem); }
int ListingDocumentType::instructionIndex(address_t address) { return this->index(address, ListingItem::InstructionItem); }
int ListingDocumentType::symbolIndex(address_t address) { return this->index(address, ListingItem::SymbolItem); }

ListingItem* ListingDocumentType::itemAt(size_t i) const
{
    if(i >= this->size())
        return nullptr;

    return this->at(i).get();
}

int ListingDocumentType::indexOf(address_t address)
{
    int idx = this->symbolIndex(address);

    if(idx == -1)
        idx = this->instructionIndex(address);

    return idx;
}

int ListingDocumentType::indexOf(ListingItem *item) { return Listing::indexOf(this, item); }
SymbolPtr ListingDocumentType::symbol(address_t address) const { return m_symboltable.symbol(address); }
SymbolPtr ListingDocumentType::symbol(const std::string &name) const { return m_symboltable.symbol(ListingDocumentType::normalized(name)); }
const SymbolTable *ListingDocumentType::symbols() const { return &m_symboltable; }

void ListingDocumentType::insertSorted(address_t address, u32 type)
{
    ListingItemPtr itemptr = std::make_unique<ListingItem>(address, type);

    if(type == ListingItem::FunctionItem)
    {
        auto it = Listing::insertionPoint(&m_functions, itemptr.get());
        m_functions.insert(it, itemptr.get());
    }

    auto it = Listing::insertionPoint(this, itemptr);

    if((it != this->end()) && (((*it)->address == address) && ((*it)->type == type)))
        return;

    it = this->insert(it, std::move(itemptr));
    ListingDocumentChanged ldc(it->get(), std::distance(this->begin(), it), ListingDocumentChanged::Inserted);
    changed(&ldc);
}

void ListingDocumentType::removeSorted(address_t address, u32 type)
{
    auto it = Listing::binarySearch(this, address, type);

    if(it == this->end())
        return;

    ListingDocumentChanged ldc(it->get(), std::distance(this->begin(), it), ListingDocumentChanged::Removed);
    changed(&ldc);

    if(type == ListingItem::FunctionItem)
    {
        auto it = Listing::binarySearch(&m_functions, address, type);
        m_functions.erase(it);
    }

    this->erase(it);
}

std::string ListingDocumentType::symbolName(const std::string &prefix, address_t address, const Segment *segment)
{
    std::stringstream ss;
    ss << prefix;

    if(segment)
        ss << "_" << ListingDocumentType::normalized(segment->name);

    ss << "_" << std::hex << address;
    return ss.str();
}

} // namespace REDasm
