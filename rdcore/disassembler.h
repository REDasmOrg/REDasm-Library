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
        void prepare(const MemoryBufferPtr& buffer, const std::string& filepath, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler);
        bool load(const RDLoaderBuildParams* buildparams);

    public:
        bool disassembleFunction(rd_address address);
        const char* getFunctionHexDump(rd_address address, rd_address* resaddress) const;

    public: // Engine/Algorithm
        void setWeak(bool b);
        bool isWeak() const;
        bool busy() const;
        void enqueue(rd_address address);
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
