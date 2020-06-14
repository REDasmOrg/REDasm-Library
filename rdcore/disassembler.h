#pragma once

#include <memory>
#include "object.h"
#include "plugin/loader.h"
#include "engine/engine.h"
#include "engine/referencetable.h"
#include "engine/stringfinder.h"
#include "engine/algorithm/algorithm.h"
#include "support/utils.h"

class Disassembler: public Object
{

    public:
        Disassembler(const RDLoaderRequest* request, RDLoaderPlugin* ploader, RDAssemblerPlugin* passembler);
        ~Disassembler();
        RDAssemblerPlugin* assembler() const;
        Loader* loader() const;
        SafeAlgorithm& algorithm();
        SafeDocument& document() const;
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

    public: // Engine/Algorithm
        bool decode(rd_address address, RDInstruction** instruction);
        void checkOperands(const RDInstruction* instruction);
        void checkOperand(const RDInstruction* instruction, const RDOperand* op);
        void enqueueAddress(const RDInstruction* instruction, rd_address address);
        void enqueue(rd_address address);

    public: // Assembler
        std::string registerName(const RDInstruction* instruction, register_t r) const;
        bool decode(BufferView* view, RDInstruction* instruction) const;
        bool encode(RDEncodedInstruction* encoded) const;
        void emulate(const RDInstruction* instruction);
        size_t addressWidth() const;
        size_t bits() const;

    public: // References
        size_t getReferences(rd_address address, const rd_address** references) const;
        size_t getTargets(rd_address address, const rd_address** targets) const;
        RDLocation getTarget(rd_address address) const;
        size_t getTargetsCount(rd_address address) const;
        size_t getReferencesCount(rd_address address) const;
        void pushReference(rd_address address, rd_address refby);
        void popReference(rd_address address, rd_address refby);
        void pushTarget(rd_address address, rd_address refby);
        void popTarget(rd_address address, rd_address refby);

    public: // Symbols
        RDLocation dereference(rd_address address) const;
        rd_type markLocation(rd_address address, rd_address fromaddress);
        rd_type markPointer(rd_address address, rd_address fromaddress);
        size_t markTable(rd_address startaddress, rd_address fromaddress, size_t count);

    public:
        bool readAddress(rd_address address, size_t size, u64 *value) const;

    private:
        BufferView* getFunctionBytes(rd_address& address) const;
        template<typename T> const T* readStringT(rd_address address, size_t* len) const;

    private:
        std::unique_ptr<Engine> m_engine;
        std::unique_ptr<Loader> m_loader;
        RDAssemblerPlugin* m_passembler;
        ReferenceTable m_references;
        SafeAlgorithm m_algorithm;
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
