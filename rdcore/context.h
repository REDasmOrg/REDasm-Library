#pragma once

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
#include "object.h"

class Disassembler;
class PluginManager;
class Analyzer;
class Assembler;
class Loader;
class MemoryBuffer;
class Surface;

typedef std::shared_ptr<Analyzer> AnalyzerPtr;
template<typename T> struct AnalyzerSorter { bool operator ()(const T& a1, const T& a2) const { return a1->plugin()->order < a2->plugin()->order; }; };
template<typename T> struct AnalyzerComparator { bool operator ()(const T& a1, const T& a2) const { return a1->plugin()->execute == a2->plugin()->execute; }; };

class Context: public Object
{
    private:
        typedef SortedContainer<AnalyzerPtr, AnalyzerComparator<AnalyzerPtr>, AnalyzerSorter<AnalyzerPtr>, true> AnalyzerPtrList;
        typedef SortedContainer<const Analyzer*, AnalyzerComparator<const Analyzer*>, AnalyzerSorter<const Analyzer*>, true> AnalyzerList;
        typedef std::unordered_map<std::string, RDEntry*> PluginMap;

    public:
        Context();
        ~Context();
        bool busy() const;
        size_t bits() const;
        size_t addressWidth() const;
        void setIgnoreProblems(bool ignore);
        void setUserData(const std::string& s, uintptr_t userdata);
        uintptr_t getUserData(const std::string& s) const;

    public: // Loader
        void findLoaderEntries(const RDLoaderRequest* req, Callback_LoaderEntry callback, void* userdata);
        Loader* loader() const;
        SafeDocument& document() const;

    public: // Assembler
        void findAssemblerEntries(Callback_AssemblerEntry callback, void* userdata) const;
        const RDEntryAssembler* findAssemblerEntry(const RDEntryLoader* entryloader, std::string* res) const;
        Assembler* assembler() const;

    public: // Analyzer
        void getAnalyzers(Callback_Analyzer callback, void* userdata) const;
        void selectAnalyzer(const Analyzer* panalyzer, bool select);
        const AnalyzerList& selectedAnalyzers() const;

    public: // Disassembler
        bool needsWeak() const;
        Disassembler* buildDisassembler(const RDLoaderRequest* req, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler);
        const DocumentNet* net() const;
        Disassembler* disassembler() const;
        MemoryBuffer* buffer() const;
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

    public:
        bool commandExecute(const char* command, const RDArguments* arguments);

    private:
        void loadAnalyzers();

    private:
        std::unordered_map<std::string, std::string> m_proposedassembler; // LoaderID -> AssemblerID
        std::unordered_map<std::string, uintptr_t> m_userdata;
        std::shared_ptr<MemoryBuffer> m_buffer;
        std::unique_ptr<Disassembler> m_disassembler;
        std::unique_ptr<PluginManager> m_pluginmanager;

    private:
        Surface* m_activesurface{nullptr};
        rd_flag m_flags{ContextFlags_None};
        PluginMap m_commands;
        AnalyzerPtrList m_analyzers;
        AnalyzerList m_selectedanalyzers;
        UniqueContainer<std::string> m_problems;
        bool m_ignoreproblems{false};
};
