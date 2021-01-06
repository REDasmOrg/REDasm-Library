#include "document.h"
#include "../plugin/assembler.h"
#include "../support/utils.h"
#include "../context.h"

Document::Document(const MemoryBufferPtr& buffer, Context* ctx): DocumentData(buffer, ctx) { m_net = std::make_unique<DocumentNet>(ctx); }
DocumentNet* Document::net() const { return m_net.get(); }

const Type* Document::type(const RDDocumentItem* item) const
{
    auto* itemdata = this->itemData(item);
    return itemdata ? itemdata->type.get() : nullptr;
}

bool Document::pointer(rd_address address, rd_type type, const std::string& name)
{
    if(!this->block(address, this->context()->addressWidth(), name, type, SymbolFlags_Pointer))
        return false;

    auto loc = this->dereference(address);
    if(loc.valid) this->checkLocation(address, loc.address);
    return loc.valid;
}

bool Document::typeName(rd_address address, const std::string& q)
{
    RDDatabaseValue v;
    if(!this->context()->database()->query(q, &v)) return false;
    return this->type(address, CPTR(const Type, v.t));
}

bool Document::type(rd_address address, const Type* type) { return this->type(address, type, 0); }

void Document::checkLocation(rd_address fromaddress, rd_address address)
{
    if(fromaddress == address) return; // Ignore self references
    if(!this->segment(address, nullptr)) return;

    RDSymbol symbol;

    if(this->symbol(address, &symbol))
    {
        if(fromaddress != RD_NVAL)
        {
            m_net->addRef(fromaddress, address); // Just add the reference

            if(HAS_FLAG(&symbol, SymbolFlags_Pointer))
            {
                auto loc = this->dereference(address);
                const char* name = this->name(address);

                if(name && loc.valid && this->symbol(loc.address, &symbol))
                    this->updateComments(fromaddress, loc.address, name, symbol.type, symbol.flags);
            }
            else
                this->updateComments(fromaddress, address, nullptr, symbol.type, symbol.flags);
        }

        return;
    }

    this->markLocation(fromaddress, address);
}

bool Document::type(rd_address address, const Type* type, int level)
{
    if(!type) return false;

    //if(m_itemdata[address].type == q) return false;

    if(auto* st = dynamic_cast<const StructureType*>(type))
    {
        RDDocumentItem item = RD_DOCITEM(address, DocumentItemType_Type);
        m_itemdata[item].type.reset(type->clone(this->context()));
        m_itemdata[item].level = level;
        rd_address fieldaddress = address;

        for(const auto& [n, f] : st->fields())
        {
            this->type(fieldaddress, f, level + 1);

            auto loc = this->dereference(fieldaddress);
            if(loc.valid) this->checkLocation(fieldaddress, loc.address); // Check the pointed location...
            else this->checkLocation(address, fieldaddress);              // ...or ust add a reference to the field
            fieldaddress += f->size();
        }

        this->replace(address, DocumentItemType_Type);
    }
    else if(auto* at = dynamic_cast<const ArrayType*>(type))
    {
        RDDocumentItem item = RD_DOCITEM(address, DocumentItemType_Type);
        m_itemdata[item].type.reset(type->clone(this->context()));
        m_itemdata[item].level = level;
        rd_address itemaddress = address;

        for(size_t i = 0; i < at->itemsCount(); i++)
        {
            this->type(itemaddress, at->type());
            itemaddress += at->type()->size();
        }
    }
    else if(auto* st = dynamic_cast<const StringType*>(type))
    {
        TypePtr cst(st->clone(this->context()));
        static_cast<StringType*>(cst.get())->calculateSize(address);

        switch(cst->type())
        {
            case Type_AsciiString:
                if(!this->asciiString(address, cst->size(), cst->name(), SymbolFlags_Field)) return false;
                break;

            case Type_WideString:
                if(!this->wideString(address, cst->size(), cst->name(), SymbolFlags_Field)) return false;
                break;

            default: return false;
        }

        RDDocumentItem item = RD_DOCITEM(address, DocumentItemType_Symbol);
        m_itemdata[item].type = std::move(cst);
        m_itemdata[item].level = level;
    }
    else if(auto* nt = dynamic_cast<const NumericType*>(type))
    {
        if(!this->data(address, nt->size(), SymbolTable::name(address, nt->name()), SymbolFlags_Field))
            return false;

        RDDocumentItem item = RD_DOCITEM(address, DocumentItemType_Symbol);
        m_itemdata[item].type.reset(type->clone(this->context()));
        m_itemdata[item].level = level;
    }
    else
        return false;

    return true;
}

void Document::updateComments(rd_address fromaddress, rd_address address, const char* symbolname, rd_type type, rd_flag flags)
{
    if(type == SymbolType_String)
    {
        if(flags & SymbolFlags_WideString)
        {
            if(symbolname)
                this->autoComment(address, std::string("=> ") + symbolname + ": " + Utils::quoted(this->readWString(address)));
            else
                this->autoComment(fromaddress, "WIDE STRING: " + Utils::quoted(this->readWString(address)));
        }
        else
        {
            if(symbolname)
                this->autoComment(address, std::string("=> ") +  symbolname + ": " + Utils::quoted(this->readString(address)));
            else
                this->autoComment(fromaddress, "STRING: " + Utils::quoted(this->readString(address)));
        }
    }
    else if((type == SymbolType_Import) && symbolname)
        this->autoComment(fromaddress, std::string("=> IMPORT: ") + symbolname);
    else if((flags & SymbolFlags_Export) && symbolname)
        this->autoComment(fromaddress, std::string("=> EXPORT: ") + symbolname);
}

void Document::markPointer(rd_address fromaddress, rd_address address)
{
    RDLocation loc = this->dereference(address);
    if(!loc.valid) return;

    this->pointer(address, SymbolType_Data, std::string());
    RDSymbol symbol;

    if(!this->symbol(loc.address, &symbol))
    {
        this->markLocation(RD_NVAL, loc.address); // Don't generate autocomments and xrefs automatically
        if(!this->symbol(loc.address, &symbol)) return;
    }

    const char* symbolname = this->name(loc.address);
    if(!symbolname) return;
    m_net->addRef(fromaddress, address);
    this->updateComments(fromaddress, symbol.address, symbolname, symbol.type, symbol.flags);

    if(fromaddress != RD_NVAL) m_net->addRef(fromaddress, loc.address, ReferenceFlags_Indirect);
}

void Document::markLocation(rd_address fromaddress, rd_address address)
{
    auto* assembler = this->context()->assembler();
    if(!assembler) return;

    RDBufferView view;
    rd_flag flags = SymbolFlags_None;

    if(this->markString(address, &flags)) // Is it a string?
    {
        if(fromaddress == RD_NVAL) return;
        this->updateComments(fromaddress, address, nullptr, SymbolType_String, flags);
    }
    else if(this->view(address, &view)) // It belongs to a mapped area?
    {
        RDLocation loc = this->dereference(address);
        if(loc.valid) this->markPointer(fromaddress, address);
        else this->data(address, assembler->addressWidth(), std::string());
    }
    else // Mapped but BSS Segment
        this->data(address, assembler->addressWidth(), std::string());

    if(fromaddress != RD_NVAL) m_net->addRef(fromaddress, address);
}

bool Document::markString(rd_address address, rd_flag* resflags)
{
    RDBlock block;
    if(!this->block(address, &block)) return false;

    RDBufferView view;
    if(!this->view(address, &view)) return false;
    view.size = BlockContainer::size(&block); // Resize view to block size

    size_t totalsize = 0;
    rd_flag flags = StringFinder::categorize(this->context(), &view, &totalsize);
    if(resflags) *resflags = flags;
    return StringFinder::checkAndMark(this->context(), address, flags, totalsize);
}
