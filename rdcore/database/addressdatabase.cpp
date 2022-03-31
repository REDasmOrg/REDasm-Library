#include "addressdatabase.h"
#include "../support/demangler.h"
#include "../support/utils.h"
#include "../document/document.h"
#include "../context.h"

AddressDatabase::AddressDatabase(Context* context): Object(context) { }

rd_address AddressDatabase::getAddress(const std::string& label) const
{
    auto it = m_labels.find(Demangler::demangled(label));
    return (it != m_labels.end()) ? it->second : RD_NVAL;
}

std::optional<std::string> AddressDatabase::getLabel(rd_address address) const
{
    auto* e = m_entries.find(address);
    return e ? std::make_optional(e->label) : std::nullopt;
}

size_t AddressDatabase::getLabels(const rd_address** addresses) const { return m_entries.data(addresses); }

size_t AddressDatabase::getLabelsByFlag(rd_flag flag, const rd_address** addresses) const
{
    auto it = m_labelflags.find(flag);
    return (it != m_labelflags.end()) ? it->second.data(addresses) : 0;
}

void AddressDatabase::setLabel(rd_address address, const std::string& label, rd_flag flags)
{
    auto e = this->getEntry(address);
    if(!e->weak && this->context()->isWeak()) return;

    e->weak = this->context()->isWeak();
    e->label = Demangler::demangled(label);
    e->flags |= flags;

    m_labels[e->label] = address;
    if(!flags) return;

    for(rd_flag f = 1; f < AddressFlags_Length; f <<= 1)
    {
        if(!(flags & f)) continue;

        if((f & AddressFlags_AsciiString) || (f & AddressFlags_WideString))
           m_labelflags[AddressFlags_String].insert(address);

        m_labelflags[f].insert(address);
    }

    spdlog::info("AddressDatabase::setLabel({:x}, '{}', {:x})", address, label, flags);
}

bool AddressDatabase::updateLabel(rd_address address, const std::string& label)
{
    if(label.empty()) return false;

    auto* e = m_entries.find(address);
    if(!e) return false;

    auto lit = m_labels.find(label);
    if(lit != m_labels.end()) return lit->second == address;

    e->label = label;
    m_labels.erase(e->label);
    m_labels[label] = address;

    spdlog::info("AddressDatabase::updateLabel({:x}, '{}')", address, label);
    return true;
}

bool AddressDatabase::findLabel(const std::string& q, rd_address* resaddress) const { return this->findLabelR(Utils::wildcardToRegex(q), resaddress); }

bool AddressDatabase::findLabelR(const std::string& q, rd_address* resaddress) const
{
    for(const auto& [label, addr] : m_labels)
    {
        if(!Utils::matchRegex(label, q)) continue;
        if(resaddress) *resaddress = addr;
        return true;
    }

    return false;
}

size_t AddressDatabase::findLabels(const std::string& q, const rd_address** resaddresses) const { return this->findLabelsR(Utils::wildcardToRegex(q), resaddresses); }

size_t AddressDatabase::findLabelsR(const std::string& q, const rd_address** resaddresses) const
{
    for(const auto& [label, addr] : m_labels)
    {
        if(!Utils::matchRegex(label, q)) continue;
        m_result.push_back(addr);
    }

    if(resaddresses) *resaddresses = m_result.data();
    return m_result.size();
}

bool AddressDatabase::isWeak(rd_address address) const
{
    if(auto* entry = m_entries.find(address); entry)
        return entry->weak;

    return false;
}

size_t AddressDatabase::assemblerToIndex(const std::string& assembler) const
{
    if(assembler.empty()) return 0;
    auto idx = m_assemblers.indexOf(assembler);
    return idx == RD_NVAL ? 0 : idx + 1;
}

std::optional<std::string> AddressDatabase::indexToAssembler(size_t index) const
{
    if(!index)
    {
        if(m_lastassembler) return m_lastassembler;
        return this->indexToAssembler(1);
    }

    index--;

    if(index < m_assemblers.size())
    {
        m_lastassembler = m_assemblers.at(index);
        return m_lastassembler;
    }

    return std::nullopt;
}

size_t AddressDatabase::pushAssembler(const std::string& assembler)
{
    m_assemblers.insert(assembler);
    return m_assemblers.indexOf(assembler) + 1;
}

rd_flag AddressDatabase::getFlags(rd_address address) const
{
    auto* e = m_entries.find(address);
    return e ? e->flags : AddressFlags_None;
}

void AddressDatabase::updateFlags(rd_address address, rd_flag flags, bool set)
{
    auto* e = m_entries.find(address);
    if(!e) return;

    if(set) e->flags |= flags;
    else e->flags &= ~flags;
}

bool AddressDatabase::setTypeField(rd_address address, const Type* type, int indent, const std::string& name)
{
    if(!type) return false;

    auto* e = this->getEntry(address);
    e->typefield.reset(type->clone(this->context()));
    e->label = name;
    e->indent = indent;
    e->flags |= AddressFlags_TypeField;

    return true;
}

void AddressDatabase::setType(rd_address address, const Type* type, const std::string& label)
{
    if(!type) return;

    auto* e = this->getEntry(address);
    e->type.reset(type->clone(this->context()));
    if(!label.empty()) e->type->setName(label);
    e->label = e->type->name();
    e->flags |= AddressFlags_Type;

    m_labels[e->label] = address;
}

const Type* AddressDatabase::getTypeField(rd_address address, int* indent) const
{
    auto* e = m_entries.find(address);
    if(!e) return nullptr;
    if(indent) *indent = e->indent;
    return e->typefield.get();
}

const Type* AddressDatabase::getType(rd_address address) const
{
    auto* e = m_entries.find(address);
    return e ? e->type.get() : nullptr;
}

AddressDatabase::Comments AddressDatabase::getComments(rd_address address) const
{
    auto* e = m_entries.find(address);
    return e ? e->comments : Comments{ };
}

void AddressDatabase::addComment(rd_address address, const std::string& s)
{
    auto* e = this->getEntry(address);
    e->comments.push_back(s);
}

void AddressDatabase::setComments(rd_address address, const Comments& c)
{
    auto* e = this->getEntry(address);
    e->comments = c;
}

AddressDatabase::Entry* AddressDatabase::getEntry(rd_address address)
{
    auto* e = m_entries.find(address);
    if(e) return e;

    m_entries.insert(address, Entry{ });
    return m_entries.find(address);
}
