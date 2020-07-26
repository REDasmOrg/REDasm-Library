#pragma once

#include <rdapi/disassembler.h>
#include <memory>
#include "object.h"
#include "plugin/loader.h"
#include "plugin/assembler.h"
#include "engine/engine.h"
#include "engine/referencetable.h"
#include "engine/stringfinder.h"
#include "engine/algorithm/algorithm.h"
#include "document/documentnet.h"
#include "support/utils.h"
#include "rdil/rdil.h"

class Disassembler: public Object
{
    public:
        Disassembler(const RDLoaderRequest* request, RDLoaderPlugin* ploader, RDAssemblerPlugin* passembler);
        ~Disassembler();
        Assembler* assembler() const;
        Loader* loader() const;
        SafeAlgorithm& algorithm();
        SafeDocument& document() const;
        const DocumentNet* net() const;
        MemoryBuffer* buffer() const;
        bool needsWeak() const;
        bool busy() const;
        void disassembleAddress(rd_address address);
        void disassemble();
        void stop();

    public:
        const char* getFunctionHexDump(rd_address address, RDSymbol* symbol) const;
        const char* getHexDump(rd_address address, size_t size) const;
        const char16_t* readWString(rd_address address, size_t* len) const;
        const char* readString(rd_address address, size_t* len) const;
        std::string readWString(rd_address address, size_t len = RD_NPOS) const; // Internal C++ Helper
        std::string readString(rd_address address, size_t len = RD_NPOS) const;  // Internal C++ Helper
        void disassembleRDIL(rd_address startaddress, Callback_DisassembleRDIL cbrdil, void* userdata);

    public: // Engine/Algorithm
        const ILCPU* ilcpu() const;
        bool decode(rd_address address, RDInstruction** instruction);
        void checkOperands(const RDInstruction* instruction);
        void checkOperand(const RDInstruction* instruction, const RDOperand* op);
        void enqueueAddress(rd_address address, const RDInstruction* instruction);

    public: // Engine/Algorithm - Flow
        void forkContinue(rd_address address, const RDInstruction* frominstruction);
        void forkBranch(rd_address address, const RDInstruction* instruction);
        void next(const RDInstruction* frominstruction);

    public: // Assembler
        RDInstruction* emitRDIL(const RDInstruction* instruction, size_t* len);
        std::string registerName(const RDInstruction* instruction, const RDOperand* op, rd_register_id r) const;
        bool decode(BufferView* view, RDInstruction* instruction) const;
        bool encode(RDEncodedInstruction* encoded) const;
        void emulate(const RDInstruction* instruction);
        void rdil(const RDInstruction* instruction);

    public: // References
        size_t getReferences(rd_address address, const rd_address** references) const;
        size_t getTargets(rd_address address, const rd_address** targets) const;
        RDLocation getTarget(rd_address address) const;
        size_t getTargetsCount(rd_address address) const;
        size_t getReferencesCount(rd_address address) const;
        void pushReference(rd_address address, rd_address refby);
        void popReference(rd_address address, rd_address refby);
        void pushTarget(rd_address address, rd_address refby, rd_type type, bool condition);
        void popTarget(rd_address address, rd_address refby, rd_type type);

    public: // Symbols
        RDLocation dereference(rd_address address) const;
        rd_type markLocation(rd_address address, rd_address fromaddress);
        rd_type markPointer(rd_address address, rd_address fromaddress);
        size_t markTable(rd_address startaddress, rd_address fromaddress, size_t count);

    public: /* *** NEW *** */
        void unlinkNext(rd_address address);

    public:
        bool readAddress(rd_address address, size_t size, u64 *value) const;

    private:
        BufferView* getFunctionBytes(rd_address& address) const;
        template<typename T> const T* readStringT(rd_address address, size_t* len) const;

    private:
        std::unique_ptr<Engine> m_engine;
        std::unique_ptr<Loader> m_loader;
        std::unique_ptr<Assembler> m_assembler;
        ReferenceTable m_references;
        SafeAlgorithm m_algorithm;
        DocumentNet m_net;
};

template<typename T>
const T* Disassembler::readStringT(rd_address address, size_t* len) const
{
    size_t clen = 0, maxlen = RD_NPOS;
    if(len && *len) maxlen = *len;

    std::unique_ptr<BufferView> view(m_loader->view(address));
    const T* pstart = reinterpret_cast<T*>(view->data());

    for(size_t i = 0 ; (i < maxlen) && !view->empty(); i++, view->advance(sizeof(T))) {
        if(!StringFinder::toAscii(*reinterpret_cast<T*>(view->data()), nullptr)) break;
        clen++;
    }

    if(len) *len = clen;
    return clen ? reinterpret_cast<const T*>(pstart) : nullptr;
}
