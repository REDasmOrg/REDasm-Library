#include "disassembler.h"
#include "support/error.h"
#include "support/sugar.h"
#include "support/utils.h"
#include "eventdispatcher.h"
#include "context.h"
#include "builtin/graph/functiongraph.h"
#include "support/utils.h"
#include <climits>

Disassembler::Disassembler(const RDLoaderRequest* request, RDLoaderPlugin* ploader, RDAssemblerPlugin* passembler): m_passembler(passembler)
{
    rd_ctx->setDisassembler(this);
    m_loader = std::make_unique<Loader>(request, ploader);
    m_algorithm = SafeAlgorithm(new Algorithm(this));
}

Disassembler::~Disassembler() { EventDispatcher::unsubscribeAll(); }
RDAssemblerPlugin* Disassembler::assembler() const { return m_passembler; }
Loader* Disassembler::loader() const { return m_loader.get(); }
SafeAlgorithm& Disassembler::algorithm() { return m_algorithm; }
bool Disassembler::needsWeak() const { return m_engine ? m_engine->needsWeak() : false; }
bool Disassembler::busy() const { return m_engine ? m_engine->busy() : false; }

void Disassembler::disassembleAddress(rd_address address)
{
    m_algorithm->enqueue(address);
    if(!m_engine->busy()) m_engine->execute(Engine::EngineState_Algorithm);
}

void Disassembler::disassemble()
{
    m_engine.reset(new Engine(this));

    if(!this->document()->segmentsCount()) return;

    // Preload functions for analysis
    for(size_t i = 0; i < this->document()->functionsCount(); i++)
        m_algorithm->enqueue(this->document()->functionAt(i).address);

    m_engine->execute();
}

void Disassembler::stop() { if(m_engine) m_engine->stop(); }

const char* Disassembler::getFunctionHexDump(rd_address address, RDSymbol* symbol) const
{
    static std::string hexdump;

    std::unique_ptr<BufferView> view(this->getFunctionBytes(address));
    if(!view) return nullptr;

    if(symbol && !this->document()->symbol(address, symbol)) return nullptr;

    hexdump = Utils::hexString(view.get());
    return hexdump.c_str();
}

const char* Disassembler::getHexDump(rd_address address, size_t size) const
{
    static std::string hexdump;

    std::unique_ptr<BufferView> view(m_loader->view(address, size));
    if(!view) return nullptr;

    hexdump = Utils::hexString(view.get());
    return hexdump.c_str();
}

const char16_t* Disassembler::readWString(rd_address address, size_t* len) const { return this->readStringT<char16_t>(address, len); }
const char* Disassembler::readString(rd_address address, size_t* len) const { return this->readStringT<char>(address, len); }

std::string Disassembler::readWString(rd_address address, size_t len) const
{
    const char16_t* s = this->readWString(address, &len);
    return s ? Utils::toString(std::u16string(s, len)) : std::string();
}

std::string Disassembler::readString(rd_address address, size_t len) const
{
    const char* s = this->readString(address, &len);
    return s ? std::string(s, len) : std::string();
}

bool Disassembler::decode(rd_address address, RDInstruction** instruction) { return m_algorithm->decodeInstruction(address, instruction);  }
void Disassembler::checkOperands(const RDInstruction* instruction) { m_algorithm->checkOperands(instruction); }
void Disassembler::checkOperand(const RDInstruction* instruction, const RDOperand* op) { m_algorithm->checkOperand(instruction, op); }
void Disassembler::enqueueAddress(const RDInstruction* instruction, rd_address address) { m_algorithm->enqueueAddress(instruction, address);  }
void Disassembler::enqueue(rd_address address) { m_algorithm->enqueue(address); }
size_t Disassembler::getReferences(rd_address address, const rd_address** references) const { return m_references.references(address, references); }
size_t Disassembler::getTargets(rd_address address, const rd_address** targets) const { return m_references.targets(address, targets); }
RDLocation Disassembler::getTarget(rd_address address) const { return m_references.target(address); }
size_t Disassembler::getTargetsCount(rd_address address) const { return m_references.targetsCount(address); }
size_t Disassembler::getReferencesCount(rd_address address) const { return m_references.referencesCount(address); }
void Disassembler::pushReference(rd_address address, rd_address refby) { m_references.pushReference(address, refby); }
void Disassembler::popReference(rd_address address, rd_address refby) { m_references.popReference(address, refby); }
void Disassembler::pushTarget(rd_address address, rd_address refby) { m_references.pushTarget(address, refby); }
void Disassembler::popTarget(rd_address address, rd_address refby) { m_references.popTarget(address, refby); }

RDLocation Disassembler::dereference(rd_address address) const
{
    RDLocation loc;
    loc.valid = this->readAddress(address, this->addressWidth(), &loc.value);
    return loc;
}

rd_type Disassembler::markLocation(rd_address address, rd_address fromaddress)
{
    if(!this->document()->segment(address, nullptr)) return SymbolType_None;

    RDSymbol symbol;
    rd_type type = SymbolType_Data;

    if(this->document()->symbol(address, &symbol) && IS_TYPE(&symbol, SymbolType_String))
    {
        if(HAS_FLAG(&symbol, SymbolFlags_WideString)) this->document()->autoComment(fromaddress, "WIDE STRING: " + Utils::quoted(this->readWString(address)));
        else this->document()->autoComment(fromaddress, "STRING: " + Utils::quoted(this->readString(address)));
        type = symbol.type;
    }
    else
    {
        RDLocation loc = this->dereference(address);

        if(loc.valid && this->document()->symbol(loc.address, nullptr))
            this->markPointer(address, fromaddress); // It points to another symbol
        else
            this->document()->data(address, this->addressWidth(), std::string());
    }

    this->pushReference(address, fromaddress);
    return type;
}

rd_type Disassembler::markPointer(rd_address address, rd_address fromaddress)
{
    RDLocation loc = this->dereference(address);
    if(!loc.valid) return this->markLocation(address, fromaddress);

    this->document()->pointer(address, SymbolType_Data, std::string());

    RDSymbol symbol;
    if(!this->document()->symbol(loc.address, &symbol)) return SymbolType_None;

    const char* symbolname = this->document()->name(symbol.address);
    if(!symbolname) return SymbolType_None;

    if(IS_TYPE(&symbol, SymbolType_String))
    {
        if(HAS_FLAG(&symbol, SymbolFlags_WideString)) this->document()->autoComment(fromaddress, std::string("=> ") + symbolname + ": " + Utils::quoted(this->readWString(loc.address)));
        else this->document()->autoComment(fromaddress, std::string("=> ") +  symbolname + ": " + Utils::quoted(this->readString(loc.address)));
    }
    else if(HAS_FLAG(&symbol, SymbolType_Import))
        this->document()->autoComment(fromaddress, std::string("=> IMPORT: ") + symbolname);
    else if(HAS_FLAG(&symbol, SymbolFlags_Export))
        this->document()->autoComment(fromaddress, std::string("=> EXPORT: ") + symbolname);
    else
        return SymbolType_None;

    this->pushReference(loc.address, fromaddress);
    return SymbolType_Data;
}

size_t Disassembler::markTable(rd_address startaddress, rd_address fromaddress, size_t count)
{
    rd_ctx->statusAddress("Checking address table", startaddress);

    RDSymbol symbol;
    if(this->document()->symbol(startaddress, &symbol) && HAS_FLAG(&symbol, SymbolFlags_TableItem)) return RD_NPOS;

    rd_address address = startaddress;
    std::deque<rd_address> targets;

    for(size_t i = 0 ; i < count; i++, address += this->addressWidth())
    {
        RDLocation loc = this->dereference(address);
        if(!loc.valid) break;

        rd_type currsymboltype = this->markLocation(loc.address, address);
        if(currsymboltype == SymbolType_None) break;

        this->pushReference(address, fromaddress);
        targets.push_back(loc.address);
    }

    if(targets.size() > 1)
    {
        size_t i = 0;

        for(rd_address target : targets)
        {
            this->document()->tableItem(target, startaddress, i++);
            this->pushReference(target, fromaddress);
        }
    }
    else if(targets.size() == 1)
    {
        this->document()->pointer(targets.front(), SymbolType_Data, std::string());
        this->pushReference(targets.front(), fromaddress);
    }
    else
        return 0;

    this->document()->pointer(startaddress, SymbolType_Data, std::string());
    return targets.size();
}

size_t Disassembler::addressWidth() const { return m_passembler->bits / CHAR_BIT;  }
size_t Disassembler::bits() const { return m_passembler->bits; }

bool Disassembler::decode(BufferView* view, RDInstruction* instruction) const
{
    if(!m_passembler->decode) return false;
    return m_passembler->decode(m_passembler, CPTR(RDBufferView, view), instruction);
}

void Disassembler::emulate(const RDInstruction* instruction)
{
    if(!m_passembler->emulate) return;
    m_passembler->emulate(m_passembler, CPTR(RDDisassembler, this), instruction);
}

std::string Disassembler::registerName(const RDInstruction* instruction, register_t r) const
{
    if(m_passembler->regname)
    {
        const char* rn = m_passembler->regname(m_passembler, instruction, r);
        if(rn) return rn;
    }

    return "$" + Utils::number(r);
}

bool Disassembler::encode(RDEncodedInstruction* encoded) const
{
    if(!encoded->decoded || !m_passembler->encode) return false;
    return m_passembler->encode(m_passembler, encoded);
}

bool Disassembler::readAddress(rd_address address, size_t size, u64* value) const
{
    std::unique_ptr<BufferView> view(m_loader->view(address));
    if(!view) return false;

    switch(size)
    {
        case 1:  if(value) *value = *reinterpret_cast<u8*>(view->data());  break;
        case 2:  if(value) *value = *reinterpret_cast<u16*>(view->data()); break;
        case 4:  if(value) *value = *reinterpret_cast<u32*>(view->data()); break;
        case 8:  if(value) *value = *reinterpret_cast<u64*>(view->data()); break;
        default: rd_ctx->problem("Invalid size: " + Utils::number(size)); return false;
    }

    return true;
}

BufferView* Disassembler::getFunctionBytes(rd_address& address) const
{
    RDLocation loc = this->document()->functionStart(address);
    if(!loc.valid) return nullptr;

    const auto* graph = this->document()->graph(loc.address);
    if(!graph) return nullptr;

    const RDGraphNode* nodes = nullptr;
    size_t c = graph->nodes(&nodes);

    RDDocumentItem startitem{ }, enditem{ };

    for(size_t i = 0; i < c; i++)
    {
        const auto* fbb = reinterpret_cast<const FunctionBasicBlock*>(graph->data(nodes[i])->p_data);
        if(!fbb) return nullptr;

        if(IS_TYPE(&startitem, DocumentItemType_None) || (startitem.address > fbb->startaddress))
        {
            if(!fbb->getStartItem(&startitem)) REDasmError("Cannot find start item");
        }

        if(IS_TYPE(&enditem, DocumentItemType_None) || (enditem.address < fbb->endaddress))
        {
            if(!fbb->getEndItem(&enditem)) REDasmError("Cannot find end item");
        }
    }

    if(IS_TYPE(&startitem, DocumentItemType_None) || IS_TYPE(&enditem, DocumentItemType_None)) return nullptr;

    address = loc.address;
    return this->loader()->view(startitem.address, (enditem.address - startitem.address) + 1);
}

SafeDocument& Disassembler::document() const { return m_loader->document(); }
MemoryBuffer* Disassembler::buffer() const { return m_loader->buffer(); }
