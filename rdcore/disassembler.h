#pragma once

#include <memory>
#include "plugin/loader.h"
#include "plugin/assembler.h"
#include "engine/engine.h"
#include "engine/algorithm/algorithm.h"
#include "document/document.h"
#include "support/utils.h"

class Disassembler: public Object
{
    public:
        Disassembler(Context* ctx);
        Assembler* assembler() const;
        Loader* loader() const;
        SafeAlgorithm& algorithm();
        SafeDocument& document() const;
        DocumentNet* net();
        MemoryBuffer* buffer() const;
        bool view(rd_address address, size_t size, RDBufferView* view) const;
        void prepare(const MemoryBufferPtr& buffer, const std::string& filepath, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler);
        bool load();

    public:
        bool createFunction(rd_address address, const char* name);
        const char* getFunctionHexDump(rd_address address, RDSymbol* symbol) const;

    public: // Engine/Algorithm
        bool needsWeak() const;
        bool busy() const;
        void enqueue(rd_address address);
        bool disassembleFunction(rd_address address, const char* name);
        void disassembleBlock(const RDBlock* block);
        void disassembleAt(rd_address address);
        void disassemble();
        void stop();

    public: // Assembler
        bool encode(RDEncodedInstruction* encoded) const;

    private:
        bool getFunctionBytes(rd_address& address, RDBufferView* view) const;

    private:
        std::unique_ptr<Engine> m_engine;
        std::unique_ptr<Loader> m_loader;
        std::unique_ptr<Assembler> m_assembler;
        SafeAlgorithm m_algorithm;
};
