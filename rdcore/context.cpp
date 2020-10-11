#include "context.h"
#include "config.h"
#include "support/utils.h"
#include "eventdispatcher.h"
#include "plugin/analyzer.h"
#include "plugin/loader.h"
#include "plugin/interface/pluginmanager.h"
#include "builtin/analyzer/unexploredanalyzer.h"
#include "builtin/analyzer/functionanalyzer.h"
#include "builtin/analyzer/stringsanalyzer.h"
#include "builtin/loader/binary.h"
#include "disassembler.h"

Context::Context(): EventDispatcher(this) { m_pluginmanager = std::make_unique<PluginManager>(this); }
Context::~Context() { this->enqueue<RDEventArgs>(Event_ContextFree, this); }
bool Context::busy() const { return m_disassembler ? m_disassembler->busy() : false; }
size_t Context::bits() const { return m_disassembler ? m_disassembler->assembler()->bits() : CHAR_BIT; }
size_t Context::addressWidth() const { return m_disassembler ? m_disassembler->assembler()->addressWidth() : 1; }
const Context::AnalyzerList& Context::selectedAnalyzers() const { return m_selectedanalyzers; }
bool Context::needsWeak() const { return m_disassembler ? m_disassembler->needsWeak() : false; }

bool Context::commandExecute(const char* command, const RDArguments* arguments)
{
    auto it = m_commands.find(command);

    if((it == m_commands.end()) || !it->second)
    {
        this->log("Cannot find command " + Utils::quoted(command));
        return false;
    }

    RDEntryCommand* pcommand = reinterpret_cast<RDEntryCommand*>(std::addressof(it->second));

    //FIXME: if(!pcommand->execute(pcommand, arguments))
    //FIXME: {
    //FIXME:     this->log("Command execution " + Utils::quoted(command) + " failed");
    //FIXME:     return false;
    //FIXME: }

    return true;
}

void Context::loadAnalyzers()
{
    m_analyzers.clear();
    m_selectedanalyzers.clear();

    for(const RDEntry* entry : m_pluginmanager->analyzers())
    {
        const auto* entryanalyzer = reinterpret_cast<const RDEntryAnalyzer*>(entry);
        auto analyzer = std::make_shared<Analyzer>(entryanalyzer, this);
        if(!analyzer->isEnabled()) continue;

        if(HAS_FLAG(entryanalyzer, AnalyzerFlags_Selected)) m_selectedanalyzers.insert(analyzer.get());
        m_analyzers.insert(analyzer);
    }
}

void Context::selectAnalyzer(const Analyzer* analyzer, bool select)
{
    if(select) m_selectedanalyzers.insert(analyzer);
    else m_selectedanalyzers.remove(analyzer);
}

void Context::setIgnoreProblems(bool ignore) { m_ignoreproblems = ignore; }
void Context::setUserData(const std::string& s, uintptr_t userdata) { m_userdata[s] = userdata; }

uintptr_t Context::getUserData(const std::string& s) const
{
    auto it = m_userdata.find(s);
    return it != m_userdata.end() ? it->second : 0;
}

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

const RDEntryAssembler* Context::findAssemblerEntry(const RDEntryLoader* entryloader, std::string* res) const
{
    if(!entryloader) return nullptr;
    auto it = m_proposedassembler.find(entryloader->id);
    if(it == m_proposedassembler.end()) return nullptr;
    if(res) *res = it->second;
    return m_pluginmanager->findAssembler(it->second);
}

Disassembler* Context::buildDisassembler(const RDLoaderRequest* req, const RDEntryLoader* entryloader, const RDEntryAssembler* entryassembler)
{
    if(m_disassembler) return m_disassembler.get();

    if(!entryassembler)
    {
        std::string assemblerid;
        entryassembler = this->findAssemblerEntry(entryloader, &assemblerid);

        if(!entryassembler)
        {
            if(assemblerid.empty()) this->log("Cannot find assembler for " + Utils::quoted(entryloader->id));
            else this->log("Cannot find assembler " + Utils::quoted(assemblerid) + " for loader " + Utils::quoted(entryloader->id));
            return nullptr;
        }

        entryassembler = m_pluginmanager->selectAssembler(entryassembler->id);
    }

    const RDEntryLoader* loaderentry = m_pluginmanager->selectLoader(entryloader->id);

    if(!loaderentry)
    {
        this->log("Cannot find loader " + Utils::quoted(entryloader->id));
        return nullptr;
    }

    m_buffer = std::shared_ptr<MemoryBuffer>(CPTR(MemoryBuffer, req->buffer)); // Take ownership
    m_disassembler = std::make_unique<Disassembler>(this);
    if(!m_disassembler->load(m_buffer, req->filepath, entryloader, entryassembler)) return nullptr;

    this->loadAnalyzers();
    return m_disassembler.get();
}

SafeDocument& Context::document() const { return m_disassembler->loader()->document(); }
const DocumentNet* Context::net() const { return m_disassembler ? m_disassembler->net() : nullptr; }
Disassembler* Context::disassembler() const { return m_disassembler.get(); }
Assembler* Context::assembler() const { return m_disassembler ? m_disassembler->assembler() : nullptr; }
Loader* Context::loader() const { return m_disassembler ? m_disassembler->loader() : nullptr; }
MemoryBuffer* Context::buffer() const { return m_disassembler ? m_disassembler->buffer() : nullptr; }
void Context::disassembleAt(rd_address address) { if(m_disassembler) m_disassembler->disassembleAt(address); }
void Context::disassemble() { if(m_disassembler) m_disassembler->disassemble(); }

void Context::setFlags(rd_flag flags, bool set)
{
    rd_flag oldflags = m_flags;

    if(set) m_flags |= flags;
    else m_flags &= ~flags;

    if(m_flags != oldflags)
        this->enqueue<RDEventArgs>(RDEvents::Event_ContextFlagsChanged, this);
}

rd_flag Context::flags() const { return m_flags; }
bool Context::hasFlags(rd_flag flags) const { return m_flags & flags; }
bool Context::hasProblems() const { return !m_problems.empty(); }
size_t Context::problemsCount() const { return m_problems.size(); }
void Context::getProblems(RD_ProblemCallback callback, void* userdata) const { for(const std::string& problem : m_problems) callback(problem.c_str(), userdata); }
void Context::problem(const std::string& s) { if(!m_ignoreproblems) m_problems.insert(s); }
