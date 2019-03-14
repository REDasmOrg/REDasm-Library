#ifndef PLUGINS_H
#define PLUGINS_H

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

LoaderList getLoaders(const LoadRequest& request, bool skipbinaries = false);
const LoaderPlugin_Entry* getLoader(const std::string& id);
const AssemblerPlugin_Entry* getAssembler(const std::string &id);
void setLoggerCallback(const Runtime::LogCallback &logcb);
void setStatusCallback(const Runtime::LogCallback& logcb);
void setProgressCallback(const Runtime::ProgressCallback &pcb);
void init(const std::string &temppath = std::string(), const std::string &searchpath = std::string());

}

#endif // PLUGINS_H
