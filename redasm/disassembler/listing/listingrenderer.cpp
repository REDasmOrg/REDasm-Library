#include "listingrenderer.h"
#include <impl/disassembler/listing/listingrenderer_impl.h>
#include "../../disassembler/disassembler.h"
#include "../../plugins/assembler/assembler.h"
#include "../../plugins/loader/loader.h"
#include "../../support/utils.h"
#include <regex>

#define INDENT_WIDTH         2
#define STRING_THRESHOLD     48
#define HEX_ADDRESS(address) Utils::hex(address, this->disassembler()->assembler()->bits())

namespace REDasm {

ListingRenderer::ListingRenderer(Disassembler *disassembler): m_pimpl_p(new ListingRendererImpl(this, disassembler)) { }

void ListingRenderer::render(size_t start, size_t count, void *userdata)
{
    auto lock = s_lock_safe_ptr(this->document());
    const ListingCursor* cur = lock->cursor();
    size_t end = start + count, line = start;
    std::string word = this->getCurrentWord();

    PIMPL_P(ListingRenderer);

    for(size_t i = 0; line < std::min(lock->size(), end); i++, line++)
    {
        RendererLine rl;
        rl.userdata = userdata;
        rl.documentindex = line;
        rl.index = i;
        rl.highlighted = cur->currentLine() == line;

        p->getRendererLine(lock, line, rl);

        if(this->cursor()->isLineSelected(line))
            p->highlightSelection(rl);
        else
            p->highlightWord(rl, word);

        if(rl.highlighted)
            p->blinkCursor(rl);

        this->renderLine(rl);
    }
}

Disassembler *ListingRenderer::disassembler() const { PIMPL_P(const ListingRenderer); return p->m_disassembler; }
const ListingDocument &ListingRenderer::document() const { PIMPL_P(const ListingRenderer); return p->m_document; }
ListingDocument &ListingRenderer::document() { PIMPL_P(ListingRenderer); return p->m_document; }

const Symbol *ListingRenderer::symbolUnderCursor()
{
    auto lock = REDasm::s_lock_safe_ptr(this->document());
    return lock->symbol(this->getCurrentWord());
}

std::string ListingRenderer::wordFromPosition(const ListingCursor::Position &pos, ListingRenderer::Range *wordpos)
{
    RendererLine rl;
    this->getRendererLine(pos.first, rl);

    for(const RendererFormat& rf : rl.formats)
    {
        if(!rf.contains(pos.second))
            continue;

        std::string word = rl.formatText(rf);

        if(this->document()->symbol(word))
        {
            if(wordpos)
                *wordpos = std::make_pair(rf.start, rf.end);

            return word;
        }
    }

    // Fallback to word matching
    std::regex rgxword(REDASM_WORD_REGEX);
    auto it = std::sregex_token_iterator(rl.text.begin(), rl.text.end(), rgxword);
    auto end = std::sregex_token_iterator();

    for(; it != end; it++)
    {
        size_t start = static_cast<size_t>(it->first - rl.text.begin());
        size_t end = static_cast<size_t>(it->second - rl.text.begin() - 1);

        if((pos.second < start) || (pos.second > end))
            continue;

        if(wordpos)
            *wordpos = std::make_pair(start, end);

        return *it;
    }

    if(wordpos)
        *wordpos = std::make_pair(1, 0);

    return std::string();
}

std::string ListingRenderer::getCurrentWord() { PIMPL_P(ListingRenderer); return this->wordFromPosition(p->m_cursor->currentPosition()); }

size_t ListingRenderer::getLastColumn(size_t line)
{
    RendererLine rl;
    this->getRendererLine(line, rl);
    size_t len = rl.length();

    if(!len)
        return 0;

    return len - 1;
}

std::string ListingRenderer::getLine(size_t line)
{
    RendererLine rl;
    this->getRendererLine(line, rl);
    return rl.text;
}

std::string ListingRenderer::getSelectedText()
{
    auto lock = s_lock_safe_ptr(this->document());
    const ListingCursor* cur = lock->cursor();

    if(!cur->hasSelection())
        return std::string();

    const ListingCursor::Position& startpos = cur->startSelection();
    const ListingCursor::Position& endpos = cur->endSelection();
    std::string copied;

    PIMPL_P(ListingRenderer);

    if(startpos.first != endpos.first)
    {
        u64 line = startpos.first;

        while(line <= endpos.first)
        {
            RendererLine rl;
            p->getRendererLine(lock, line, rl);
            std::string s = rl.text;

            if(line == startpos.first)
                copied += s.substr(startpos.second);
            else if(line == endpos.first)
                copied += s.substr(0, endpos.second + 1);
            else
                copied += s;

            copied += "\n";
            line++;
        }
    }
    else
    {
        RendererLine rl;
        p->getRendererLine(lock, startpos.first, rl);
        copied = rl.text.substr(startpos.second, endpos.second - startpos.second + 1);
    }

    return copied;
}

bool ListingRenderer::hasFlag(ListingRendererFlags flag) const { PIMPL_P(const ListingRenderer); return p->hasFlag(flag); }
void ListingRenderer::setFlags(ListingRendererFlags flags) { PIMPL_P(ListingRenderer); p->setFlags(flags); }

bool ListingRenderer::getRendererLine(size_t line, RendererLine &rl)
{
    PIMPL_P(ListingRenderer);
    auto lock = document_s_lock(this->document());
    return p->getRendererLine(lock, line, rl);
}

void ListingRenderer::renderSegment(const document_s_lock& lock, const ListingItem *item, RendererLine &rl)
{
    this->printer()->segment(lock->segment(item->address()), [&](const std::string& line) {
        rl.push(line, "segment_fg");
    });
}

void ListingRenderer::renderFunction(const document_s_lock& lock, const ListingItem *item, RendererLine& rl)
{
    if(rl.ignoreflags || !this->hasFlag(ListingRendererFlags::HideSegmentAndAddress))
        this->renderAddressIndent(lock, item, rl);

    this->printer()->function(lock->symbol(item->address()), [&](const std::string& pre, const std::string& sym, const std::string& post) {
        if(!pre.empty())
            rl.push(pre, "function_fg");

        rl.push(sym, "function_fg");

        if(!post.empty())
            rl.push(post, "function_fg");
    });
}

void ListingRenderer::renderInstruction(const document_s_lock& lock, const ListingItem *item, RendererLine &rl)
{
    InstructionPtr instruction = lock->instruction(item->address());

    this->renderAddress(lock, item, rl);
    this->renderIndent(rl, 3);
    this->renderMnemonic(instruction, rl);
    this->renderOperands(instruction, rl);
    this->renderComments(lock, item, rl);
}

void ListingRenderer::renderSymbol(const document_s_lock& lock, const ListingItem *item, RendererLine &rl)
{
    Loader* loader = this->disassembler()->loader();
    Assembler* assembler = this->disassembler()->assembler();
    const Symbol* symbol = lock->symbol(item->address());

    PIMPL_P(ListingRenderer);

    if(symbol->is(SymbolType::Code)) // Label or Callback
    {
        const Segment* segment = lock->segment(symbol->address);

        if(segment->is(SegmentType::Bss))
        {
            this->renderAddress(lock, item, rl);
            this->renderIndent(rl);
            rl.push(symbol->name, "label_fg");
            rl.push(" <").push("dynamic branch", "label_fg").push(">");
        }
        else
        {
            if(!rl.ignoreflags && this->hasFlag(ListingRendererFlags::HideSegmentAndAddress))
                this->renderIndent(rl, 2);
            else
                this->renderAddressIndent(lock, item, rl);

            rl.push(symbol->name, "label_fg").push(":");
        }
    }
    else // Data
    {
        const Segment* segment = lock->segment(item->address());
        this->renderAddress(lock, item, rl);
        this->renderIndent(rl, 3);
        rl.push(symbol->name, "label_fg");
        this->renderIndent(rl);

        if(!segment->is(SegmentType::Bss) && loader->offset(symbol->address).valid)
        {
            if(symbol->is(SymbolType::Pointer))
            {
                if(p->renderSymbolPointer(lock, symbol, rl))
                    return;
            }

            if(symbol->is(SymbolType::WideStringMask))
                rl.push(Utils::quoted(this->disassembler()->readWString(symbol, STRING_THRESHOLD)), "string_fg");
            else if(symbol->is(SymbolType::StringMask))
                rl.push(Utils::quoted(this->disassembler()->readString(symbol, STRING_THRESHOLD)), "string_fg");
            else if(symbol->is(SymbolType::ImportMask))
                rl.push("<").push("import", "label_fg").push(">");
            else
            {
                u64 value = 0;
                this->disassembler()->readAddress(symbol->address, assembler->addressWidth(), &value);
                rl.push(Utils::hex(value, this->disassembler()->assembler()->bits()), this->document()->segment(value) ? "pointer_fg" : "data_fg");
            }
        }
        else if(symbol->is(SymbolType::ImportMask))
            rl.push("<").push("import", "label_fg").push(">");
        else
            rl.push("??", "data_fg");
    }
}

void ListingRenderer::renderMeta(const document_s_lock &lock, const ListingItem *item, RendererLine &rl)
{
    this->renderAddressIndent(lock, item, rl);
    auto metaitem = lock->meta(item);
    rl.push(metaitem.type + " ", "meta_fg").push(metaitem.name, "comment_fg");
}

void ListingRenderer::renderType(const document_s_lock &lock, const ListingItem *item, RendererLine &rl)
{
    this->renderAddressIndent(lock, item, rl);
    rl.push(".type ", "meta_fg").push(lock->type(item), "comment_fg");
}

void ListingRenderer::renderAddress(const document_s_lock &lock, const ListingItem *item, RendererLine &rl)
{
    if(!rl.ignoreflags && (this->hasFlag(ListingRendererFlags::HideSegmentName) && !this->hasFlag(ListingRendererFlags::HideAddress)))
        rl.push(HEX_ADDRESS(item->address()), "address_fg");
    else if(rl.ignoreflags || !this->hasFlag(ListingRendererFlags::HideAddress))
    {
        const Segment* segment = lock->segment(item->address());
        rl.push((segment ? segment->name : "unk") + ":" + HEX_ADDRESS(item->address()), "address_fg");
    }
}

void ListingRenderer::renderMnemonic(const InstructionPtr &instruction, RendererLine &rl)
{
    std::string mnemonic = instruction->mnemonic + " ";

    if(instruction->isInvalid())
        rl.push(mnemonic, "instruction_invalid");
    else if(instruction->is(REDasm::InstructionType::Stop))
        rl.push(mnemonic, "instruction_stop");
    else if(instruction->is(REDasm::InstructionType::Nop))
        rl.push(mnemonic, "instruction_nop");
    else if(instruction->is(REDasm::InstructionType::Call))
        rl.push(mnemonic, "instruction_call");
    else if(instruction->is(REDasm::InstructionType::Compare))
        rl.push(mnemonic, "instruction_compare");
    else if(instruction->is(REDasm::InstructionType::Jump))
    {
        if(instruction->is(REDasm::InstructionType::Conditional))
            rl.push(mnemonic, "instruction_jmp_c");
        else
            rl.push(mnemonic, "instruction_jmp");
    }
    else
        rl.push(mnemonic);
}

void ListingRenderer::renderOperands(const InstructionPtr &instruction, RendererLine &rl)
{
    this->printer()->out(instruction, [&](const REDasm::Operand* op, const std::string& opsize, const std::string& opstr) {
        if(!op) {
            rl.push(opstr, "immediate_fg");
            return;
        }

        if(op->index > 0)
            rl.push(", ");

        if(!opsize.empty())
            rl.push(opsize + " ");

        if(op->isNumeric())
        {
            if(op->is(REDasm::OperandType::Memory))
                rl.push(opstr, "memory_fg");
            else
                rl.push(opstr, "immediate_fg");
        }
        else if(op->is(REDasm::OperandType::Displacement))
            rl.push(opstr, "displacement_fg");
        else if(op->is(REDasm::OperandType::Register))
            rl.push(opstr, "register_fg");
        else
            rl.push(opstr);
    });
}

void ListingRenderer::renderComments(const document_s_lock &lock, const ListingItem* item, RendererLine &rl)
{
    std::string s = lock->comment(item);

    if(s.empty())
        return;

    rl.push("   # " + ListingRendererImpl::escapeString(s), "comment_fg");
}

void ListingRenderer::renderAddressIndent(const document_s_lock& lock, const ListingItem* item, RendererLine &rl)
{
    const Segment* segment = lock->segment(item->address());
    size_t count = this->disassembler()->assembler()->bits() / 4;

    if(segment)
        count += segment->name.length();

    rl.push(std::string(count + INDENT_WIDTH, ' '));
}

void ListingRenderer::renderIndent(RendererLine &rl, int n) { rl.push(std::string(n * INDENT_WIDTH, ' ')); }
Printer *ListingRenderer::printer() const { PIMPL_P(const ListingRenderer); return p->m_printer.get(); }
ListingCursor *ListingRenderer::cursor() const { PIMPL_P(const ListingRenderer); return p->m_cursor; }

} // namespace REDasm
