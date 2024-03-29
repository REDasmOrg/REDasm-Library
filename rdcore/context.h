#pragma once

#define DEFAULT_MIN_STRING 4

#include <unordered_set>
#include <memory>
#include <string>
#include <rdapi/context.h>
#include <rdapi/plugin/loader.h>
#include <rdapi/plugin/analyzer.h>
#include <rdapi/plugin/command.h>
#include <rdapi/plugin/assembler/assembler.h>
#include "containers/sortedcontainer.h"
#include "containers/uniquecontainer.h"
#include "document/document_fwd.h"
#include "database/database.h"
#include "object.h"

class AddressDatabase;
class Disassembler;
class PluginManager;
class Analyzer;
class Assembler;
class Loader;
class MemoryBuffer;
class Surface;

typedef std::shared_ptr<Analyzer> AnalyzerPtr;
template<typename T> struct AnalyzerSorter { bool operator ()(const T& a1, const T& a2) const; };
template<typename T> struct AnalyzerComparator { bool operator ()(const T& a1, const T& a2) const; };

class Context: public Object
{
    public:
        struct SurfaceState {
            size_t mnemonicendcol{8}; // Give some indentation by default
            size_t instrstartcol{0};
        };

    private:
        typedef SortedContainer<AnalyzerPtr, AnalyzerComparator<AnalyzerPtr>, AnalyzerSorter<AnalyzerPtr>, true> AnalyzerPtrList;
        typedef SortedContainer<const Analyzer*, AnalyzerComparator<const Analyzer*>, AnalyzerSorter<const Analyzer*>, true> AnalyzerList;
        typedef std::unordered_map<std::string, RDEntry*> PluginMap;

    public:
        Context();
        ~Context();
        Database* database() const;
        AddressDatabase* addressDatabase() const;
        const SurfaceState& surfaceState() const;
        SurfaceState& surfaceState();
        bool disassembling() const;
        bool busy() const;
        size_t bits() const;
        size_t addressWidth() const;
        bool matchLoader(const std::string& q) const;
        bool matchAssembler(const std::string& q) const;
        void setIgnoreProblems(bool ignore);
        void setUserData(const std::string& s, uintptr_t userdata);
        uintptr_t getUserData(const std::string& s) const;
        void setCompilerABI(rd_type t);
        rd_type compilerABI() const;
        void setCompilerCC(rd_type t);
        rd_type compilerCC() const;
        void setMinString(size_t s);
        size_t minString() const;

    public: // Document
        RDLocation functionStart(rd_address address) const;

    public: // Loader
        void findLoaderEntries(const RDLoaderRequest* req, Callback_LoaderEntry callback, void* userdata);
        Loader* loader() const;
        SafeDocument& document() const;

    public: // Assembler
        Assembler* getAssembler(rd_address address) const;
        Assembler* getAssembler(const std::string& id) const;
        void findAssemblerEntries(Callback_AssemblerEntry callback, void* userdata) const;
        const RDEntryAssembler* findAssemblerEntry(const RDEntryLoader* entryloader, std::string* res) const;
        Assembler* assembler() const;

    public: // Analyzer
        void getAnalyzers(Callback_Analyzer callback, void* userdata) const;
        void disableAnalyzer(const std::string& analyzerid);
        void selectAnalyzer(const Analyzer* panalyzer, bool select);
        bool isAnalyzerSelected(const Analyzer* panalyzer) const;
        const AnalyzerList& selectedAnalyzers() const;

    public: // Command
        bool executeCommand(const char* cmd, const RDArguments* a) const;

    public: // Disassembler
        const DocumentNet* net() const;
        DocumentNet* net();
        bool bind(const RDLoaderRequest* req, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler);
        Disassembler* disassembler() const;
        MemoryBuffer* buffer() const;
        bool isWeak() const;
        void setWeak(bool b);
        void disassembleBlock(const RDBlock* block);
        void disassembleAt(rd_address address);
        void disassemble();

    public:
        Surface* activeSurface() const;
        void setActiveSurface(Surface* sf);

    public: // Flags
        rd_flag flags() const;
        void setFlags(rd_flag flags, bool set);
        bool hasFlag(rd_flag flags) const;

    public: // Problems
        bool hasProblems() const;
        void getProblems(RD_ProblemCallback callback, void* userdata) const;
        void problem(const std::string& s);
        size_t problemsCount() const;

    private:
        void loadAnalyzers();

    private:
        mutable std::unordered_map<std::string, std::unique_ptr<Assembler>> m_assemblers;
        std::unordered_map<std::string, std::string> m_proposedassembler; // LoaderID -> AssemblerID
        std::unordered_map<std::string, uintptr_t> m_userdata;
        std::shared_ptr<MemoryBuffer> m_buffer;
        std::unique_ptr<Disassembler> m_disassembler;
        std::unique_ptr<PluginManager> m_pluginmanager;

    private:
        std::unique_ptr<Database> m_database;
        std::unique_ptr<AddressDatabase> m_addrdatabase;
        std::pair<rd_type, rd_type> m_compilerabi{CompilerABI_Unknown, CompilerCC_Unknown};
        SurfaceState m_surfacestate;
        Surface* m_activesurface{nullptr};
        rd_flag m_flags{ContextFlags_None};
        PluginMap m_commands;
        AnalyzerPtrList m_analyzers;
        std::unordered_set<std::string> m_disabledanalyzers;
        AnalyzerList m_selectedanalyzers;
        UniqueContainer<std::string> m_problems;
        size_t m_minstring{DEFAULT_MIN_STRING};
        bool m_ignoreproblems{false};
};

class WeakScope
{
    public:
        WeakScope(Context* ctx): m_ctx(ctx), m_oldweak(ctx->isWeak()) { m_ctx->setWeak(true); }
        ~WeakScope() { m_ctx->setWeak(m_oldweak); }

    private:
        Context* m_ctx;
        bool m_oldweak;
};
