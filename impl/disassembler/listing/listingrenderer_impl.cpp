#include "listingrenderer_impl.h"
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/disassembler/disassembler.h>
#include <redasm/context.h>

namespace REDasm {

ListingRendererImpl::ListingRendererImpl(ListingRenderer *q): m_pimpl_q(q), m_flags(ListingRendererFlags::Normal)
{
    m_cursor = r_doc->cursor();
    m_printer = r_asm->createPrinter();
}

bool ListingRendererImpl::renderSymbolPointer(const document_s_lock &lock, const Symbol *symbol, RendererLine &rl) const
{
    u64 value = 0;

    if(!r_disasm->readAddress(symbol->address, r_asm->addressWidth(), &value))
        return false;

    const Symbol* ptrsymbol = lock->symbol(value);

    if(!ptrsymbol)
        return false;

    rl.push(ptrsymbol->name, ptrsymbol->isLocked() ? "locked_fg" : "label_fg");
    return true;
}

bool ListingRendererImpl::getRendererLine(const document_s_lock &lock, size_t line, RendererLine &rl)
{
    ListingItem* item = lock->itemAt(std::min(line, lock->lastLine()));

    if(!item)
        return false;

    PIMPL_Q(ListingRenderer);

    if(item->is(ListingItemType::SegmentItem))
        q->renderSegment(lock, item, rl);
    else if(item->is(ListingItemType::FunctionItem))
        q->renderFunction(lock, item, rl);
    else if(item->is(ListingItemType::InstructionItem))
        q->renderInstruction(lock, item, rl);
    else if(item->is(ListingItemType::SymbolItem))
        q->renderSymbol(lock, item, rl);
    else if(item->is(ListingItemType::MetaItem))
        q->renderMeta(lock, item, rl);
    else if(item->is(ListingItemType::TypeItem))
        q->renderType(lock, item, rl);
    else if(item->is(ListingItemType::EmptyItem))
        rl.push(" ");
    else
        rl.push("Unknown Type: " + String::number(static_cast<size_t>(item->type())));

    return true;
}

void ListingRendererImpl::highlightSelection(RendererLine &rl)
{
    if(rl.text.empty())
        return;

    const REDasm::ListingCursor::Position& startsel = m_cursor->startSelection();
    const REDasm::ListingCursor::Position& endsel = m_cursor->endSelection();

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
    if(!m_cursor->active())
        return;

    rl.format(m_cursor->currentColumn(), m_cursor->currentColumn(), "cursor_fg", "cursorbg");
}

void ListingRendererImpl::highlightWord(RendererLine &rl, const String word)
{
    if(word.empty())
        return;

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

bool ListingRendererImpl::hasFlag(ListingRendererFlags flag) const { return m_flags & flag; }
void ListingRendererImpl::setFlags(ListingRendererFlags flags) { m_flags = flags; }

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

} // namespace REDasm
