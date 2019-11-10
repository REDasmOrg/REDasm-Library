#include "listingrenderer.h"
#include <impl/disassembler/listing/listingrenderer_impl.h>
#include <algorithm>
#include "../../disassembler/model/functiongraph.h"
#include "../../disassembler/disassembler.h"
#include "../../plugins/assembler/assembler.h"
#include "../../plugins/loader/loader.h"
#include "../../support/utils.h"
#include "../../types/regex.h"
#include "../../context.h"

#define INDENT_WIDTH         2
#define STRING_THRESHOLD     48
#define HEX_ADDRESS(address) String::hex(address, r_asm->bits())

namespace REDasm {

RendererLine::RendererLine(bool ignoreflags): userdata(nullptr), documentindex(0), index(0), highlighted(false), ignoreflags(ignoreflags) { }
String RendererLine::formatText(const RendererFormat &rf) const { return text.substring(rf.start, rf.length()); }
size_t RendererLine::length() const { return text.size(); }
bool RendererFormat::empty() const { return this->length() == 0; }
bool RendererFormat::contains(size_t pos) const { return (pos >= start) && (pos <= end); }
bool RendererFormat::equals(size_t start, size_t end) const { return (start == this->start) && (end == this->end); }

size_t RendererFormat::length() const
{
    if((start == REDasm::npos) || (end == REDasm::npos))
        return 0;

    return start <= end ? (end - start + 1) : 0;
}

RendererFormatList::RendererFormatList(): m_pimpl_p(new RendererFormatListImpl()) { }
const RendererFormat &RendererFormatList::at(size_t pos) const { PIMPL_P(const RendererFormatList); return p->at(pos); }
RendererFormat &RendererFormatList::at(size_t pos) { PIMPL_P(RendererFormatList); return p->at(pos); }
void RendererFormatList::append(const RendererFormat &rf) { PIMPL_P(RendererFormatList); p->append(rf); }
size_t RendererFormatList::insert(size_t idx, const RendererFormat &rf) { PIMPL_P(RendererFormatList); return p->insert(idx, rf); }

size_t RendererFormatList::indexFromPos(size_t pos) const
{
    PIMPL_P(const RendererFormatList);

    for(size_t i = 0; i < p->size(); i++)
    {
        if(p->at(i).contains(pos))
            return i;
    }

    return REDasm::npos;
}

size_t RendererFormatList::size() const { PIMPL_P(const RendererFormatList); return p->size(); }

size_t RendererFormatList::erase(size_t start, size_t end)
{
    PIMPL_P(RendererFormatList);

    size_t pos = REDasm::npos;

    for(size_t i = start; i < end; i++)
        pos = p->eraseAt(start);

    return pos;
}

RendererLine &RendererLine::push(const String &text, const String &fgstyle, const String &bgstyle)
{
    size_t start = this->text.size();
    formats.append({ start, start + text.size() - 1, fgstyle, bgstyle});
    this->text += text;
    return *this;
}

size_t RendererLine::unformat(size_t start, size_t end)
{
    size_t begidx = formats.indexFromPos(start);
    size_t endidx = formats.indexFromPos(end);

    RendererFormat begrf = formats.at(begidx), endrf = formats.at(endidx);
    size_t idx = formats.erase(begidx, endidx + 1);

    begrf.end = start - 1; // Shrink first part
    endrf.start = end + 1; // Shrink last part

    if(!begrf.empty())
    {
        idx = formats.insert(idx, begrf);
        idx++;
    }

    if(!endrf.empty())
        idx = formats.insert(idx, endrf);

    return idx;
}

RendererLine &RendererLine::format(size_t start, size_t end, const String &fgstyle, const String &bgstyle)
{
    if(text.empty() || (start >= text.size()))
        return *this;

    end = std::min(end, text.size() - 1);

    size_t idx = this->unformat(start, end);
    formats.insert(idx, { start, end, fgstyle, bgstyle });
    return *this;
}

ListingRenderer::ListingRenderer(): m_pimpl_p(new ListingRendererImpl(this)) { }

void ListingRenderer::render(size_t start, size_t count, void *userdata)
{
    auto lock = s_lock_safe_ptr(r_docnew);
    const ListingCursor& cur = lock->cursor();
    size_t end = start + count, line = start;
    String word = this->getCurrentWord();

    PIMPL_P(ListingRenderer);

    for(size_t i = 0; line < std::min(lock->itemsCount(), end); i++, line++)
    {
        RendererLine rl;
        rl.userdata = userdata;
        rl.documentindex = line;
        rl.index = i;
        rl.highlighted = cur.currentLine() == line;

        p->getRendererLine(lock, line, rl);

        if(r_docnew->cursor().isLineSelected(line)) p->highlightSelection(rl);
        else p->highlightWord(rl, word);

        if(rl.highlighted) p->blinkCursor(rl);
        this->renderLine(rl);
    }
}

const Symbol *ListingRenderer::symbolUnderCursor()
{
    auto lock = REDasm::s_lock_safe_ptr(r_docnew);
    return lock->symbol(this->getCurrentWord());
}

String ListingRenderer::wordFromPosition(const ListingCursor::Position &pos, ListingRenderer::Range *wordpos)
{
    RendererLine rl;
    this->getRendererLine(pos.line, rl);

    for(size_t i = 0; i < rl.formats.size(); i++)
    {
        const RendererFormat& rf = rl.formats.at(i);
        if(!rf.contains(pos.column)) continue;

        String word = rl.formatText(rf);

        if(r_docnew->symbol(word))
        {
            if(wordpos) *wordpos = std::make_pair(rf.start, rf.end);
            return word;
        }
    }

    // Fallback to word matching
    Regex rgxword(REDASM_WORD_REGEX);
    auto m = rgxword.matchAll(rl.text);

    while(m.hasNext())
    {
        auto match = m.next();
        size_t start = match.start;
        size_t end = match.end;

        if((pos.column < start) || (pos.column > end))
            continue;

        if(wordpos) *wordpos = std::make_pair(start, end);
        return match.value;
    }

    if(wordpos) *wordpos = std::make_pair(1, 0);
    return String();
}

String ListingRenderer::getCurrentWord() { return this->wordFromPosition(r_docnew->cursor().currentPosition()); }

size_t ListingRenderer::getLastColumn(size_t line)
{
    RendererLine rl;
    this->getRendererLine(line, rl);
    size_t len = rl.length();
    if(!len) return 0;

    return len - 1;
}

String ListingRenderer::getLine(size_t line)
{
    RendererLine rl;
    this->getRendererLine(line, rl);
    return rl.text;
}

String ListingRenderer::getSelectedText()
{
    auto lock = s_lock_safe_ptr(r_docnew);
    const ListingCursor& cur = lock->cursor();
    if(!cur.hasSelection()) return String();

    const ListingCursor::Position& startpos = cur.startSelection();
    const ListingCursor::Position& endpos = cur.endSelection();
    String copied;

    PIMPL_P(ListingRenderer);

    if(startpos.line != endpos.line)
    {
        size_t line = startpos.line;

        while(line <= endpos.line)
        {
            RendererLine rl;
            p->getRendererLine(lock, line, rl);
            String s = rl.text;

            if(line == startpos.line) copied += s.substring(startpos.column);
            else if(line == endpos.line) copied += s.substring(0, endpos.column + 1);
            else copied += s;

            copied += "\n";
            line++;
        }
    }
    else
    {
        RendererLine rl;
        p->getRendererLine(lock, startpos.line, rl);
        copied = rl.text.substring(startpos.column, endpos.column - startpos.column + 1);
    }

    return copied;
}

bool ListingRenderer::hasFlag(ListingRendererFlags flag) const { PIMPL_P(const ListingRenderer); return p->hasFlag(flag); }
void ListingRenderer::setFlags(ListingRendererFlags flags) { PIMPL_P(ListingRenderer); p->setFlags(flags); }

bool ListingRenderer::getRendererLine(size_t line, RendererLine &rl) const
{
    PIMPL_P(const ListingRenderer);
    auto lock = REDasm::s_lock_safe_ptr(r_docnew);
    return p->getRendererLine(lock, line, rl);
}

void ListingRenderer::renderSegment(const document_s_lock_new& lock, const ListingItem& item, RendererLine &rl) const
{
    this->printer()->segment(lock->segment(item.address_new), [&](const String& line) {
        rl.push(line, "segment_fg");
    });
}

void ListingRenderer::renderFunction(const document_s_lock_new& lock, const ListingItem& item, RendererLine& rl) const
{
    if(rl.ignoreflags || !this->hasFlag(ListingRendererFlags::HideSegmentAndAddress))
        this->renderAddressIndent(lock, item, rl);

    const Symbol* symbol = lock->symbol(item.address_new);
    if(!symbol) return;

    this->printer()->function(symbol, [&](const String& pre, const String& sym, const String& post) {
        if(!pre.empty()) rl.push(pre, "function_fg");
        rl.push(sym, "function_fg");
        if(!post.empty()) rl.push(post, "function_fg");
    });
}

void ListingRenderer::renderInstruction(const document_s_lock_new& lock, const ListingItem& item, RendererLine &rl) const
{
    CachedInstruction instruction = lock->instruction(item.address_new);
    this->renderAddress(lock, item, rl);

    if(!this->hasFlag(ListingRendererFlags::HideSegmentAndAddress)) this->renderIndent(rl, 3);
    else this->renderIndent(rl, 1);

    this->renderMnemonic(instruction, rl);
    this->renderOperands(instruction, rl);
    this->renderComments(lock, item, rl);
}

void ListingRenderer::renderSymbol(const document_s_lock_new& lock, const ListingItem& item, RendererLine &rl) const
{
    PIMPL_P(const ListingRenderer);
    u64 value = 0;

    const Symbol* symbol = lock->symbol(item.address_new);

    if(!symbol)
    {
        p->renderPrologue(lock, item, rl);
        rl.push("Unknown symbol");
        return;
    }

    const Segment* segment = lock->segment(symbol->address);

    if(segment && segment->is(SegmentType::Bss))
    {
        p->renderSymbolPrologue(lock, item, symbol, rl);

        if(symbol->is(SymbolType::LabelNew))
        {
            rl.push(symbol->name, "label_fg");
            rl.push(" <").push("dynamic branch", "label_fg").push(">");
        }
        else
            rl.push("??", "data_fg");

        return;
    }

    switch(symbol->type)
    {
        case SymbolType::ImportNew:
            p->renderSymbolPrologue(lock, item, symbol, rl);
            rl.push("<").push("import", "label_fg").push(">");
            break;

        case SymbolType::StringNew:
            p->renderSymbolPrologue(lock, item, symbol, rl);
            if(symbol->hasFlag(SymbolFlags::WideString)) rl.push(r_disasm->readWString(symbol, STRING_THRESHOLD).quoted(), "string_fg");
            else rl.push(r_disasm->readString(symbol, STRING_THRESHOLD).quoted(), "string_fg");
            break;

        case SymbolType::LabelNew:
            if(!rl.ignoreflags && this->hasFlag(ListingRendererFlags::HideSegmentAndAddress)) this->renderIndent(rl, 2);
            else this->renderAddressIndent(lock, item, rl);
            rl.push(symbol->name, "label_fg").push(":");
            break;

        default:
            p->renderSymbolPrologue(lock, item, symbol, rl);
            r_disasm->readAddress(symbol->address, r_asm->addressWidth(), &value); // TODO: Check block size
            rl.push(String::hex(value, r_asm->bits()), lock->segment(value) ? "pointer_fg" : "data_fg");
            break;
    }
}

void ListingRenderer::renderMeta(const document_s_lock_new &lock, const ListingItem& item, RendererLine &rl) const
{
    this->renderAddressIndent(lock, item, rl);
    rl.push("META WIP");
    //FIXME: auto metaitem = lock->meta(item);
    //FIXME: rl.push(metaitem.type + " ", "meta_fg").push(metaitem.name, "comment_fg");
}

void ListingRenderer::renderType(const document_s_lock_new &lock, const ListingItem& item, RendererLine &rl) const
{
    this->renderAddressIndent(lock, item, rl);
    rl.push("TYPE WIP");
    //FIXME: rl.push(".type ", "meta_fg").push(lock->type(item), "comment_fg");
}

void ListingRenderer::renderSeparator(const document_s_lock_new& lock, const ListingItem& item, RendererLine &rl) const
{
    if(!rl.ignoreflags && this->hasFlag(ListingRendererFlags::HideSeparators))
        return;

    if(rl.ignoreflags || !this->hasFlag(ListingRendererFlags::HideSegmentAndAddress))
        this->renderAddressIndent(lock, item, rl);

    rl.push(String::repeated('-', SEPARATOR_LENGTH), "separator");
}

void ListingRenderer::renderUnexplored(const document_s_lock_new& lock, const ListingItem& item, RendererLine& rl) const
{
    const BlockItem* bi = lock->block(item.address_new);
    this->renderAddress(lock, item, rl);

    if(!this->hasFlag(ListingRendererFlags::HideSegmentAndAddress)) this->renderIndent(rl, 3);
    else this->renderIndent(rl, 1);

    switch(bi->size())
    {
        case 2:  rl.push("dw "); break;
        case 4:  rl.push("dd "); break;
        case 8:  rl.push("dq "); break;
        default: rl.push("db "); break;
    }

    PIMPL_P(const ListingRenderer);
    p->renderBlockBytes(bi, rl);
}

void ListingRenderer::renderAddress(const document_s_lock_new &lock, const ListingItem& item, RendererLine &rl) const
{
    if(!rl.ignoreflags && (this->hasFlag(ListingRendererFlags::HideSegment) && !this->hasFlag(ListingRendererFlags::HideAddress)))
        rl.push(HEX_ADDRESS(item.address_new), "address_fg");
    else if(rl.ignoreflags || !this->hasFlag(ListingRendererFlags::HideAddress))
    {
        const Segment* segment = lock->segment(item.address_new);
        rl.push((segment ? segment->name() : "unk") + ":" + HEX_ADDRESS(item.address_new), "address_fg");
    }
}

void ListingRenderer::renderMnemonic(const CachedInstruction &instruction, RendererLine &rl) const
{
    String mnemonic = instruction->mnemonic() + " ";

    if(instruction->isInvalid())
        rl.push(mnemonic, "instruction_invalid");
    else if(instruction->typeIs(REDasm::InstructionType::Stop))
        rl.push(mnemonic, "instruction_stop");
    else if(instruction->typeIs(REDasm::InstructionType::Nop))
        rl.push(mnemonic, "instruction_nop");
    else if(instruction->typeIs(REDasm::InstructionType::Call))
        rl.push(mnemonic, "instruction_call");
    else if(instruction->typeIs(REDasm::InstructionType::Compare))
        rl.push(mnemonic, "instruction_compare");
    else if(instruction->typeIs(REDasm::InstructionType::Jump))
    {
        if(instruction->typeIs(REDasm::InstructionType::Conditional))
            rl.push(mnemonic, "instruction_jmp_c");
        else
            rl.push(mnemonic, "instruction_jmp");
    }
    else
        rl.push(mnemonic);
}

void ListingRenderer::renderOperands(const CachedInstruction &instruction, RendererLine &rl) const
{
    this->printer()->out(instruction, [&](const REDasm::Operand* op, const String& opsize, const String& opstr) {
        if(!op) {
            rl.push(opstr, "immediate_fg");
            return;
        }

        if(op->index > 0) rl.push(", ");
        if(!opsize.empty()) rl.push(opsize + " ");

        if(Operand::isNumeric(op)) {
            if(REDasm::typeIs(op, REDasm::OperandType::Memory)) rl.push(opstr, "memory_fg");
            else rl.push(opstr, "immediate_fg");
        }
        else if(REDasm::typeIs(op, REDasm::OperandType::Displacement)) rl.push(opstr, "displacement_fg");
        else if(REDasm::typeIs(op, REDasm::OperandType::Register)) rl.push(opstr, "register_fg");
        else rl.push(opstr);
    });
}

void ListingRenderer::renderComments(const document_s_lock_new &lock, const ListingItem& item, RendererLine &rl) const
{
    String s = lock->comment(item.address_new);
    if(s.empty()) return;

    rl.push("   # " + ListingRendererImpl::escapeString(s), "comment_fg");
}

void ListingRenderer::renderAddressIndent(const document_s_lock_new& lock, const ListingItem& item, RendererLine &rl) const
{
    const Segment* segment = lock->segment(item.address_new);
    size_t count = r_asm->bits() / 4;
    if(segment) count += segment->name().size();

    rl.push(String::repeated(' ', count + INDENT_WIDTH));
}

void ListingRenderer::renderIndent(RendererLine &rl, int n) const { rl.push(String::repeated(' ', n * INDENT_WIDTH)); }
Printer *ListingRenderer::printer() const { PIMPL_P(const ListingRenderer); return p->m_printer.get(); }

} // namespace REDasm
