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
        RDAssemblerPlugin* assembler() const;
        Loader* loader() const;
        SafeAlgorithm& algorithm();
        SafeDocument& document() const;
        MemoryBuffer* buffer() const;
        bool needsWeak() const;
        bool busy() const;
        void disassembleAddress(address_t address);
        void disassemble();
        void stop();

    public:
        const char* getHexDump(address_t address, RDSymbol* symbol) const;
        const char16_t* readWString(address_t address, size_t* len) const;
        const char* readString(address_t address, size_t* len) const;
        std::string readWString(address_t address, size_t len = RD_NPOS) const; // Internal C++ Helper
        std::string readString(address_t address, size_t len = RD_NPOS) const;  // Internal C++ Helper

    public: // Engine/Algorithm
        bool decode(address_t address, RDInstruction** instruction);
        void handleOperand(const RDInstruction* instruction, const RDOperand* op);
        void enqueueAddress(const RDInstruction* instruction, address_t address);
        void enqueue(address_t address);

    public: // Assembler
        std::string registerName(const RDInstruction* instruction, register_t r) const;
        bool decode(BufferView* view, RDInstruction* instruction) const;
        bool encode(RDEncodedInstruction* encoded) const;
        void emulate(const RDInstruction* instruction);
        size_t addressWidth() const;
        size_t bits() const;

    public: // References
        size_t getReferences(address_t address, const address_t** references) const;
        size_t getTargets(address_t address, const address_t** targets) const;
        RDLocation getTarget(address_t address) const;
        size_t getTargetsCount(address_t address) const;
        size_t getReferencesCount(address_t address) const;
        void pushReference(address_t address, address_t refby);
        void popReference(address_t address, address_t refby);
        void pushTarget(address_t address, address_t refby);
        void popTarget(address_t address, address_t refby);

    public: // Symbols
        RDLocation dereference(address_t address) const;
        void markLocation(address_t fromaddress, address_t address);
        size_t markTable(const RDInstruction* instruction, address_t startaddress);

    public:
        bool readAddress(address_t address, size_t size, u64 *value) const;

    private:
        BufferView* getFunctionBytes(address_t& address) const;
        template<typename T> const T* readStringT(address_t address, size_t* len) const;

    private:
        std::unique_ptr<Engine> m_engine;
        std::unique_ptr<Loader> m_loader;
        RDAssemblerPlugin* m_passembler;
        ReferenceTable m_references;
        SafeAlgorithm m_algorithm;
};

template<typename T>
const T* Disassembler::readStringT(address_t address, size_t* len) const
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
