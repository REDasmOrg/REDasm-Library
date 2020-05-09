#include "disassembler.h"
#include "support/sugar.h"
#include "support/utils.h"
#include "context.h"
#include "builtin/graph/functiongraph.h"
#include "support/utils.h"
#include <climits>
#include <cassert>

Disassembler::Disassembler(const RDLoaderRequest* request, RDLoaderPlugin* ploader, RDAssemblerPlugin* passembler): m_passembler(passembler)
{
    rd_ctx->setDisassembler(this);
    m_loader = std::make_unique<Loader>(request, ploader);
    m_algorithm = SafeAlgorithm(new Algorithm(this));
}

RDAssemblerPlugin* Disassembler::assembler() const { return m_passembler; }
Loader* Disassembler::loader() const { return m_loader.get(); }
SafeAlgorithm& Disassembler::algorithm() { return m_algorithm; }
bool Disassembler::needsWeak() const { return m_engine ? m_engine->needsWeak() : false; }
bool Disassembler::busy() const { return m_engine ? m_engine->busy() : false; }

void Disassembler::disassembleAddress(address_t address)
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
        m_algorithm->enqueue(this->document()->functionAt(i));

    if(rd_ctx->sync() || m_engine->concurrency() == 1) rd_ctx->log("Single threaded disassembly");
    else rd_ctx->log("Disassembling with " + Utils::number(m_engine->concurrency()) + " threads");

    m_engine->execute();
}

void Disassembler::stop() { if(m_engine) m_engine->stop(); }

const char* Disassembler::getHexDump(address_t address, RDSymbol* symbol) const
{
    static std::string hexdump;

    std::unique_ptr<BufferView> view(this->getFunctionBytes(address));
    if(!view) return nullptr;

    if(symbol && !this->document()->symbol(address, symbol)) return nullptr;

    hexdump = Utils::hexString(view.get());
    return hexdump.c_str();
}

const char16_t* Disassembler::readWString(address_t address, size_t* len) const { return this->readStringT<char16_t>(address, len); }
const char* Disassembler::readString(address_t address, size_t* len) const { return this->readStringT<char>(address, len); }

std::string Disassembler::readWString(address_t address, size_t len) const
{
    const char16_t* s = this->readWString(address, &len);
    return s ? Utils::toString(std::u16string(s, len)) : std::string();
}

std::string Disassembler::readString(address_t address, size_t len) const
{
    const char* s = this->readString(address, &len);
    return s ? std::string(s, len) : std::string();
}

void Disassembler::handleOperand(const RDInstruction* instruction, const RDOperand* op) { m_algorithm->handleOperand(instruction, op); }
void Disassembler::enqueueAddress(const RDInstruction* instruction, address_t address) { m_algorithm->enqueueAddress(instruction, address);  }
void Disassembler::enqueue(address_t address) { m_algorithm->enqueue(address); }
size_t Disassembler::getReferences(address_t address, const address_t** references) const { return m_references.references(address, references); }
size_t Disassembler::getTargets(address_t address, const address_t** targets) const { return m_references.targets(address, targets); }
RDLocation Disassembler::getTarget(address_t address) const { return m_references.target(address); }
size_t Disassembler::getTargetsCount(address_t address) const { return m_references.targetsCount(address); }
size_t Disassembler::getReferencesCount(address_t address) const { return m_references.referencesCount(address); }
void Disassembler::pushReference(address_t address, address_t refby) { m_references.pushReference(address, refby); }
void Disassembler::popReference(address_t address, address_t refby) { m_references.popReference(address, refby); }
void Disassembler::pushTarget(address_t address, address_t refby) { m_references.pushTarget(address, refby); }
void Disassembler::popTarget(address_t address, address_t refby) { m_references.popTarget(address, refby); }

RDLocation Disassembler::dereference(address_t address) const
{
    RDLocation loc;
    loc.valid = this->readAddress(address, this->addressWidth(), &loc.value);
    return loc;
}

void Disassembler::markLocation(address_t fromaddress, address_t address)
{
    if(!this->document()->segment(address, nullptr)) return;

    RDSymbol symbol;

    if(this->document()->symbol(address, &symbol) && (symbol.type == SymbolType_String))
    {
        if(symbol.flags & SymbolFlags_WideString) this->document()->autoComment(fromaddress, "WIDE STRING: " + Utils::quoted(this->readWString(address)));
        else this->document()->autoComment(fromaddress, "STRING: " + Utils::quoted(this->readString(address)));
    }
    else this->document()->data(address, this->addressWidth(), std::string());

    this->pushReference(address, fromaddress);
}

size_t Disassembler::markTable(const RDInstruction* instruction, address_t startaddress)
{
    RDSymbol symbol;
    if(this->document()->symbol(startaddress, &symbol) && (symbol.flags & SymbolFlags_TableItem)) return RD_NPOS;

    address_t target = 0, address = startaddress;

    if(!this->readAddress(address, this->addressWidth(), &target))
        return 0;

    rd_ctx->statusAddress("Checking address table", startaddress);
    std::unordered_set<address_t> targets;

    while(this->readAddress(address, this->addressWidth(), &target))
    {
        RDSegment segment;

        if(!this->document()->segment(target, &segment) || !(segment.type & SegmentType_Code)) break;
        targets.insert(target);

        if(Sugar::isBranch(instruction)) this->pushTarget(target, instruction->address);
        else this->markLocation(startaddress, target);

        address += this->addressWidth();
    }

    if(targets.empty())
    {
        if(targets.size() > 1)
        {
            size_t i = 0;
            address = startaddress;

            for(auto it = targets.begin(); it != targets.end(); it++, address += this->addressWidth(), i++)
            {
                if(address == startaddress) this->document()->table(address, targets.size());
                else this->document()->tableItem(address, startaddress, i);
                this->pushReference(address, instruction->address);
                address += this->addressWidth();
            }
        }
        else
        {
            this->pushReference(startaddress, instruction->address);
            this->document()->pointer(startaddress, SymbolType_Data, std::string());
        }
    }

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

bool Disassembler::readAddress(address_t address, size_t size, u64* value) const
{
    RDSegment segment;
    if(!this->document()->segment(address, &segment)) return false;

    std::unique_ptr<BufferView> view(m_loader->view(segment));
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

BufferView* Disassembler::getFunctionBytes(address_t& address) const
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

        if((startitem.type == DocumentItemType_None) || (startitem.address > fbb->startaddress))
            assert(fbb->getStartItem(&startitem));

        if((enditem.type == DocumentItemType_None) || (enditem.address < fbb->endaddress))
            assert(fbb->getEndItem(&enditem));
    }

    if((startitem.type == DocumentItemType_None) || (enditem.type == DocumentItemType_None)) return nullptr;

    address = loc.address;
    return this->loader()->view(startitem.address, (enditem.address - startitem.address) + 1);
}

SafeDocument& Disassembler::document() const { return m_loader->document(); }
MemoryBuffer* Disassembler::buffer() const { return m_loader->buffer(); }
