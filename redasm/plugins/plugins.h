#pragma once

#include <forward_list>
#include <string>
#include <deque>
#include <map>
#include "assembler/assembler.h"
#include "loader.h"

namespace REDasm {

template<typename T> struct EntryListT { typedef std::forward_list<T> Type; };
template<typename T> struct EntryMapT { typedef std::map<std::string, T> Type; };
typedef std::deque<const LoaderPlugin_Entry*> LoaderList;

template<typename T> typename EntryMapT<T>::Type::const_iterator findPluginEntry(const std::string& id, const typename EntryMapT<T>::Type& pm)
{
    if(id.empty())
        return pm.end();

    return pm.find(id);
}

struct Plugins
{
    static LIBREDASM_EXPORT size_t loadersCount;
    static LIBREDASM_EXPORT EntryListT<LoaderPlugin_Entry>::Type loaders;
    static LIBREDASM_EXPORT EntryMapT<AssemblerPlugin_Entry>::Type assemblers;
};

template<typename T> T* createAssembler(const std::string &id)
{
    auto it = Plugins::assemblers.find(id);

    if(it != Plugins::assemblers.end())
        return static_cast<T*>(it->second.init());

    REDasm::log("Cannot find " + REDasm::quoted(id) + " assembler");
    return nullptr;
}

LoaderList getLoaders(const LoadRequest& request, bool skipbinaries = false);
const LoaderPlugin_Entry* getLoader(const std::string& id);
const AssemblerPlugin_Entry* getAssembler(const std::string &id);
void setLoggerCallback(const Context_LogCallback &logcb);
void setStatusCallback(const Context_LogCallback& logcb);
void setProgressCallback(const Context_ProgressCallback &pcb);
void init(const ContextSettings &settings);

}
