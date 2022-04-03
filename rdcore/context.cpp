#include "context.h"
#include "config.h"
#include "support/utils.h"
#include "plugin/analyzer.h"
#include "plugin/loader.h"
#include "plugin/interface/pluginmanager.h"
#include "builtin/analyzer/unexploredanalyzer.h"
#include "builtin/analyzer/functionanalyzer.h"
#include "builtin/analyzer/stringsanalyzer.h"
#include "builtin/loader/binary.h"
#include "document/document.h"
#include "database/addressdatabase.h"
#include "database/database.h"
#include "disassembler.h"

#define INSTRUCTION_START_COL 16

Context::Context(): Object(this)
{
    m_surfacestate.instrstartcol = INSTRUCTION_START_COL;

    m_pluginmanager = std::make_unique<PluginManager>(this);
    m_addrdatabase = std::make_unique<AddressDatabase>(this);
    m_database = std::make_unique<Database>(this);
    m_database->setName("Active Database");

    spdlog::info("*** Context::Context() ***");
}

Context::~Context()
{
    spdlog::info("*** Context::~Context() ***");
    this->notify<RDEventArgs>(Event_ContextFree, this);
}

Database* Context::database() const { return m_database.get(); }
AddressDatabase* Context::addressDatabase() const { return m_addrdatabase.get(); }
const Context::SurfaceState& Context::surfaceState() const { return m_surfacestate; }
Context::SurfaceState& Context::surfaceState() { return m_surfacestate; }
bool Context::disassembling() const { return m_disassembler ? m_disassembler->disassembling() : false; }
bool Context::busy() const { return m_disassembler ? m_disassembler->busy() : false; }
size_t Context::bits() const { return m_disassembler ? m_disassembler->assembler()->bits() : CHAR_BIT; }
size_t Context::addressWidth() const { return m_disassembler ? m_disassembler->assembler()->addressWidth() : 1; }

bool Context::matchLoader(const std::string& q) const
{
    if(!m_disassembler) return false;
    const char* id = m_disassembler->loader()->id();
    return id ? Utils::matchWildcard(id, q) : false;
}

bool Context::matchAssembler(const std::string& q) const
{
    if(!m_disassembler) return false;
    const char* id = m_disassembler->assembler()->id();
    return id ? Utils::matchWildcard(id, q) : false;
}

const Context::AnalyzerList& Context::selectedAnalyzers() const { return m_selectedanalyzers; }
bool Context::executeCommand(const char* cmd, const RDArguments* a) const { return cmd ? m_pluginmanager->executeCommand(cmd, a) : false; }
bool Context::isWeak() const { return m_disassembler ? m_disassembler->isWeak() : false; }
void Context::disassembleBlock(const RDBlock* block) { if(m_disassembler) m_disassembler->disassembleBlock(block); }

void Context::loadAnalyzers()
{
    for(const RDEntry* entry : m_pluginmanager->analyzers())
    {
        const auto* entryanalyzer = reinterpret_cast<const RDEntryAnalyzer*>(entry);
        if(m_disabledanalyzers.count(entryanalyzer->id)) continue;

        auto analyzer = std::make_shared<Analyzer>(entryanalyzer, this);
        if(!analyzer->isEnabled()) continue;

        if(HAS_FLAG(entryanalyzer, AnalyzerFlags_Selected))
            m_selectedanalyzers.insert(analyzer.get());

        m_analyzers.insert(analyzer);
    }
}

void Context::selectAnalyzer(const Analyzer* analyzer, bool select)
{
    if(select) m_selectedanalyzers.insert(analyzer);
    else m_selectedanalyzers.remove(analyzer);
}

bool Context::isAnalyzerSelected(const Analyzer* panalyzer) const
{
    if(!panalyzer) return false;

    for(const Analyzer* a : m_selectedanalyzers)
        if(a == panalyzer) return true;

    return false;
}

void Context::setIgnoreProblems(bool ignore) { m_ignoreproblems = ignore; }
void Context::setUserData(const std::string& s, uintptr_t userdata) { m_userdata[s] = userdata; }

uintptr_t Context::getUserData(const std::string& s) const
{
    auto it = m_userdata.find(s);
    return it != m_userdata.end() ? it->second : 0;
}

void Context::setCompilerABI(rd_type t) { m_compilerabi.first = t; }
rd_type Context::compilerABI() const { return m_compilerabi.first; }
void Context::setCompilerCC(rd_type t) { m_compilerabi.second = t; }
rd_type Context::compilerCC() const { return m_compilerabi.second; }
void Context::setMinString(size_t s) { m_minstring = std::max<size_t>(1, s); }
size_t Context::minString() const { return m_minstring; }
RDLocation Context::functionStart(rd_address address) const { return m_disassembler->document()->getFunctionStart(address); }

void Context::findLoaderEntries(const RDLoaderRequest* req, Callback_LoaderEntry callback, void* userdata)
{
    m_proposedassembler.clear();
    if(!callback || !req->filepath || !req->buffer) return;

    for(const RDEntry* entry : m_pluginmanager->loaders())
    {
        const auto* entryloader = reinterpret_cast<const RDEntryLoader*>(entry);
        const char* assemblerid = Loader::test(entryloader, req);
        if(!assemblerid) continue;

        if(!std::string(assemblerid).empty() && !m_pluginmanager->findAssembler(assemblerid))
        {
            this->log("Invalid assembler id: " + Utils::quoted(assemblerid));
            continue;
        }

        m_proposedassembler[entryloader->id] = assemblerid;
        callback(entryloader, userdata);
    }
}

void Context::findAssemblerEntries(Callback_AssemblerEntry callback, void* userdata) const
{
    for(const RDEntry* entry : m_pluginmanager->assemblers())
        callback(reinterpret_cast<const RDEntryAssembler*>(entry), userdata);
}

void Context::getAnalyzers(Callback_Analyzer callback, void* userdata) const
{
    if(!callback) return;

    std::for_each(m_analyzers.begin(), m_analyzers.end(), [&](const auto& analyzer) {
        callback(CPTR(const RDAnalyzer, analyzer.get()), userdata);
    });
}

void Context::disableAnalyzer(const std::string& analyzerid) { if(!analyzerid.empty()) m_disabledanalyzers.insert(analyzerid); }

const RDEntryAssembler* Context::findAssemblerEntry(const RDEntryLoader* entryloader, std::string* res) const
{
    if(!entryloader) return nullptr;
    auto it = m_proposedassembler.find(entryloader->id);
    if(it == m_proposedassembler.end()) return nullptr;
    if(res) *res = it->second;
    return m_pluginmanager->findAssembler(it->second);
}

bool Context::bind(const RDLoaderRequest* req, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler)
{
    spdlog::info("Context::bind({:p}, {:p}, {:p})",
                 reinterpret_cast<const void*>(req),  reinterpret_cast<const void*>(entryloader),
                 reinterpret_cast<const void*>(entryassembler));

    if(m_disassembler)
    {
        spdlog::warn("Context::bind({:p}, {:p}, {:p}): Disassembler already exists, skipping...",
                     reinterpret_cast<const void*>(req),  reinterpret_cast<const void*>(entryloader),
                     reinterpret_cast<const void*>(entryassembler));

        return m_disassembler.get();
    }

    if(!entryassembler)
    {
        std::string assemblerid;
        entryassembler = this->findAssemblerEntry(entryloader, &assemblerid);

        if(!entryassembler)
        {
            if(assemblerid.empty())
            {
                spdlog::error("Context::bind({:p}, {:p}, {:p}): AssemblerId is empty for loader '{}'",
                             reinterpret_cast<const void*>(req),  reinterpret_cast<const void*>(entryloader),
                             reinterpret_cast<const void*>(entryassembler), entryloader->id);
                this->log("Cannot find assembler for " + Utils::quoted(entryloader->id));
            }
            else
            {
                spdlog::error("Context::bind({:p}, {:p}, {:p}): Cannot find assembler for loader '{}'",
                              reinterpret_cast<const void*>(req),  reinterpret_cast<const void*>(entryloader),
                              reinterpret_cast<const void*>(entryassembler), entryloader->id);

                this->log("Cannot find assembler " + Utils::quoted(assemblerid) + " for loader " + Utils::quoted(entryloader->id));
            }

            return false;
        }

        entryassembler = m_pluginmanager->selectAssembler(entryassembler->id);
    }

    const RDEntryLoader* loaderentry = m_pluginmanager->selectLoader(entryloader->id);

    if(!loaderentry)
    {
        this->log("Cannot find loader " + Utils::quoted(entryloader->id));
        return false;
    }

    m_buffer = MemoryBufferPtr(CPTR(MemoryBuffer, req->buffer)); // Take ownership
    m_disassembler = std::make_unique<Disassembler>(this);
    m_disassembler->prepare(m_buffer, req->filepath, entryloader, entryassembler);

    m_pluginmanager->checkCommands();
    if(!m_disassembler->load(&req->buildparams)) return false;

    this->loadAnalyzers();
    return true;
}

SafeDocument& Context::document() const { return m_disassembler->loader()->document(); }

Assembler* Context::getAssembler(rd_address address) const
{
    auto assembler = this->document()->getAddressAssembler(address);

    if(!assembler)
    {
        rd_log("Cannot get assembler @ " + Utils::hex(address));
        return nullptr;
    }

    return this->getAssembler(*assembler);
}

Assembler* Context::getAssembler(const std::string& id) const
{
    spdlog::debug("Context::getAssembler('{}')", id);
    if(m_disassembler->assembler()->id() == id) return m_disassembler->assembler();

    auto it = m_assemblers.find(id);
    if(it != m_assemblers.end()) return it->second.get();

    auto* assembler = m_pluginmanager->getAssembler(id);
    if(!assembler) return nullptr;

    auto iit = m_assemblers.try_emplace(id, new Assembler(assembler, this->context()));
    return iit.first->second.get();
}

const DocumentNet* Context::net() const { return m_disassembler ? m_disassembler->document()->net() : nullptr; }
DocumentNet* Context::net() { return m_disassembler ? m_disassembler->document()->net() : nullptr; }
Disassembler* Context::disassembler() const { return m_disassembler.get(); }
Assembler* Context::assembler() const { return m_disassembler ? m_disassembler->assembler() : nullptr; }
Loader* Context::loader() const { return m_disassembler ? m_disassembler->loader() : nullptr; }
MemoryBuffer* Context::buffer() const { return m_disassembler ? m_disassembler->document()->buffer() : nullptr; }
void Context::setWeak(bool b) { if(m_disassembler) m_disassembler->setWeak(b); }
void Context::disassembleAt(rd_address address) { if(m_disassembler) m_disassembler->disassembleAt(address); }
void Context::disassemble() { if(m_disassembler) m_disassembler->disassemble(); }
Surface* Context::activeSurface() const { return m_activesurface; }

void Context::setActiveSurface(Surface* sf)
{
    if(m_activesurface == sf) return;

    m_activesurface = sf;
    this->notify<RDEventArgs>(Event_ContextSurfaceChanged, this);
}

void Context::setFlags(rd_flag flags, bool set)
{
    rd_flag oldflags = m_flags;

    if(set) m_flags |= flags;
    else m_flags &= ~flags;

    if(m_flags != oldflags)
        this->notify<RDEventArgs>(RDEvents::Event_ContextFlagsChanged, this);
}

rd_flag Context::flags() const { return m_flags; }
bool Context::hasFlag(rd_flag flags) const { return m_flags & flags; }
bool Context::hasProblems() const { return !m_problems.empty(); }
size_t Context::problemsCount() const { return m_problems.size(); }
void Context::getProblems(RD_ProblemCallback callback, void* userdata) const { for(const std::string& problem : m_problems) callback(problem.c_str(), userdata); }
void Context::problem(const std::string& s) { if(!m_ignoreproblems) m_problems.insert(s); }

template<typename T>
bool AnalyzerSorter<T>::operator ()(const T &a1, const T &a2) const { return a1->plugin()->order < a2->plugin()->order; }

template<typename T>
bool AnalyzerComparator<T>::operator ()(const T &a1, const T &a2) const { return a1->plugin()->execute == a2->plugin()->execute; }
