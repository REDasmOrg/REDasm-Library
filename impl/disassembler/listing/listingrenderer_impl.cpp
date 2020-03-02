#include "listingrenderer_impl.h"
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/disassembler/disassembler.h>
#include <redasm/support/utils.h>
#include <redasm/context.h>

namespace REDasm {

ListingRendererImpl::ListingRendererImpl(ListingRenderer *q): m_pimpl_q(q) { m_printer = r_asm->createPrinter(); }

bool ListingRendererImpl::renderSymbolPointer(const document_s_lock &lock, const Symbol *symbol, RendererLine &rl) const
{
    u64 value = 0;

    if(!r_disasm->readAddress(symbol->address, r_asm->addressWidth(), &value))
        return false;

    const Symbol* ptrsymbol = lock->symbol(value);
    if(!ptrsymbol) return false;

    rl.push(ptrsymbol->name, "label_fg");
    return true;
}

void ListingRendererImpl::renderSymbolPrologue(const document_s_lock& lock, const ListingItem& item, const Symbol* symbol, RendererLine& rl) const
{
    PIMPL_Q(const ListingRenderer);
    this->renderPrologue(lock, item, rl);
    rl.push(symbol->name, "label_fg");
    q->renderIndent(rl);
}

bool ListingRendererImpl::getRendererLine(const document_s_lock &lock, size_t line, RendererLine &rl) const
{
    if(lock->empty()) return false;

    const ListingItem& item = lock->itemAt(std::min(line, lock->itemsCount() - 1));
    if(!item.isValid()) return false;

    PIMPL_Q(const ListingRenderer);

    switch(item.type)
    {
        case ListingItem::SegmentItem:     q->renderSegment(lock, item, rl); break;
        case ListingItem::FunctionItem:    q->renderFunction(lock, item, rl); break;
        case ListingItem::InstructionItem: q->renderInstruction(lock, item, rl); break;
        case ListingItem::SymbolItem:      q->renderSymbol(lock, item, rl); break;
        case ListingItem::MetaItem:        q->renderMeta(lock, item, rl); break;
        case ListingItem::TypeItem:        q->renderType(lock, item, rl); break;
        case ListingItem::SeparatorItem:   q->renderSeparator(lock, item, rl); break;
        case ListingItem::UnexploredItem:  q->renderUnexplored(lock, item, rl); break;
        case ListingItem::EmptyItem:       rl.push(" "); break;
        default: rl.push("Unknown Type: " + String::number(static_cast<size_t>(item.type))); break;
    }

    return true;
}

void ListingRendererImpl::highlightSelection(RendererLine &rl)
{
    if(rl.text.empty())
        return;

    const REDasm::ListingCursor::Position& startsel = r_doc->cursor().startSelection();
    const REDasm::ListingCursor::Position& endsel = r_doc->cursor().endSelection();

    if(startsel.line != endsel.line)
    {
        size_t start = (rl.documentindex == startsel.line) ? startsel.column : 0;
        size_t end = (rl.documentindex == endsel.line) ? endsel.column : (rl.text.size() - 1);
        rl.format(start, end, "selection_fg", "selection_bg");
    }
    else
        rl.format(startsel.column, endsel.column, "selection_fg", "selection_bg");
}

void ListingRendererImpl::blinkCursor(RendererLine &rl)
{
    if(!r_doc->cursor().active()) return;
    rl.format(r_doc->cursor().currentColumn(), r_doc->cursor().currentColumn(), "cursor_fg", "cursorbg");
}

void ListingRendererImpl::highlightWord(RendererLine &rl, const String word)
{
    if(word.empty()) return;

    size_t pos = rl.text.indexOf(word, 0);
    std::list<size_t> locations;

    while(pos != String::npos)
    {
        locations.push_back(pos);
        pos = rl.text.indexOf(word, pos + 1);
    }

    for(size_t loc : locations)
        rl.format(loc, loc + word.size() - 1, "highlight_fg", "highlight_bg");
}

bool ListingRendererImpl::hasFlag(flag_t flag) const { return m_flags & flag; }
void ListingRendererImpl::setFlags(flag_t flags) { m_flags = flags; }

void ListingRendererImpl::renderBlockBytes(const BlockItem* bi, RendererLine& rl) const
{
    if(bi->size() > 8)
    {
        rl.push(String::hex(bi->size()), "immediate_fg");
        return;
    }

    if(!(bi->size() % 2) || (bi->size() == 1))
    {
        u64 value = 0;
        bool res = r_disasm->readAddress(bi->start, bi->size(), &value);
        rl.push(res ? String::hex(value, bi->size() * CHAR_BIT) : "??", "immediate_fg");
        return;
    }

    address_t address = bi->start;
    rl.push("[");

    for(size_t i = 0; i < bi->size(); i++, address++)
    {
        u64 value = 0;
        bool res = r_disasm->readAddress(address, 1, &value);

        if(i) rl.push(", ");
        rl.push(res ? String::hex(value, 8) : "??", "immediate_fg");
    }

    rl.push("]");
}

String ListingRendererImpl::escapeString(const String &s)
{
    String res;

    for(size_t i = 0; i < s.size(); i++)
    {
        switch(s[i])
        {
            case '\n':
                res += "\\\n";
                break;

            case '\r':
                res += "\\\r";
                break;

            case '\t':
                res += "\\\t";
                break;

            default:
                res += s[i];
                break;
        }
    }

    return res;
}

void ListingRendererImpl::renderPrologue(const document_s_lock& lock, const ListingItem& item, RendererLine& rl) const
{
    PIMPL_Q(const ListingRenderer);
    q->renderAddress(lock, item, rl);
    q->renderIndent(rl, 3);
}

} // namespace REDasm
