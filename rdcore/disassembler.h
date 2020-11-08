#pragma once

#include <rdapi/disassembler.h>
#include <memory>
#include "plugin/loader.h"
#include "plugin/assembler.h"
#include "engine/engine.h"
#include "engine/stringfinder.h"
#include "engine/algorithm/algorithm.h"
#include "document/documentnet.h"
#include "support/utils.h"

class Disassembler: public Object
{
    public:
        Disassembler(Context* ctx);
        Assembler* assembler() const;
        Loader* loader() const;
        SafeAlgorithm& algorithm();
        SafeDocument& document() const;
        const DocumentNet* net() const;
        DocumentNet* net();
        MemoryBuffer* buffer() const;
        bool view(rd_address address, size_t size, RDBufferView* view) const;
        bool load(const std::shared_ptr<MemoryBuffer>& buffer, const std::string& filepath, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler);

    public:
        bool scheduleFunction(rd_address address, const char* name);
        bool createFunction(rd_address address, const char* name);
        const char* getFunctionHexDump(rd_address address, RDSymbol* symbol) const;
        const char* getHexDump(rd_address address, size_t size) const;
        const char16_t* readWString(rd_address address, size_t* len) const;
        const char* readString(rd_address address, size_t* len) const;
        std::string readWString(rd_address address, size_t len = RD_NVAL) const; // Internal C++ Helper
        std::string readString(rd_address address, size_t len = RD_NVAL) const;  // Internal C++ Helper

    public: // Engine/Algorithm
        bool needsWeak() const;
        bool busy() const;
        void enqueue(rd_address address);
        void schedule(rd_address address);
        void disassembleAt(rd_address address);
        void disassemble();
        void stop();

    public: // Assembler
        bool encode(RDEncodedInstruction* encoded) const;

    public: // Symbols
        RDLocation dereference(rd_address address) const;
        size_t markTable(rd_address startaddress, rd_address fromaddress, size_t count);
        void checkLocation(rd_address fromaddress, rd_address address);

    private:
        void markPointer(rd_address fromaddress, rd_address address);
        void markLocation(rd_address fromaddress, rd_address address);
        bool markString(rd_address address, rd_flag* resflags);
        bool readAddress(rd_address address, size_t size, u64 *value) const;
        bool getFunctionBytes(rd_address& address, RDBufferView* view) const;
        template<typename T> const T* readStringT(rd_address address, size_t* len) const;

    private:
        std::unique_ptr<Engine> m_engine;
        std::unique_ptr<Loader> m_loader;
        std::unique_ptr<Assembler> m_assembler;
        SafeAlgorithm m_algorithm;
        DocumentNet m_net;
};

template<typename T>
const T* Disassembler::readStringT(rd_address address, size_t* len) const
{
    size_t clen = 0, maxlen = RD_NVAL;
    if(len && *len) maxlen = *len;

    RDBufferView view;
    if(!m_loader->view(address, &view)) return nullptr;
    const T* pstart = reinterpret_cast<T*>(view.data);

    for(size_t i = 0 ; (i < maxlen) && !BufferView::empty(&view); i++, BufferView::advance(&view, sizeof(T))) {
        if(!StringFinder::toAscii(*reinterpret_cast<T*>(view.data), nullptr)) break;
        clen++;
    }

    if(len) *len = clen;
    return clen ? reinterpret_cast<const T*>(pstart) : nullptr;
}
