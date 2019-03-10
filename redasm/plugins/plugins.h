#ifndef PLUGINS_H
#define PLUGINS_H

#include <forward_list>
#include <unordered_map>
#include <string>
#include <deque>
#include "format.h"
#include "assembler/assembler.h"

namespace REDasm {

template<typename T> struct EntryListT { typedef std::forward_list<T> Type; };
template<typename T> struct EntryListExtT { typedef std::deque< std::pair<T, std::string> > Type; };
template<typename T> struct EntryMapT { typedef std::unordered_map<std::string, T> Type; };

typedef EntryListExtT<FormatPlugin_Entry>::Type FormatEntryListByExt;

template<typename T> typename EntryMapT<T>::Type::const_iterator findPluginEntry(const std::string& id, const typename EntryMapT<T>::Type& pm)
{
    if(id.empty())
        return pm.end();

    return pm.find(id);
}

struct Plugins
{
    static LIBREDASM_EXPORT size_t formatsCount;
    static LIBREDASM_EXPORT EntryListT<FormatPlugin_Entry>::Type formats;
    static LIBREDASM_EXPORT EntryMapT<FormatEntryListByExt>::Type formatsByExt;
    static LIBREDASM_EXPORT EntryMapT<AssemblerPlugin_Entry>::Type assemblers;
};

bool getFormatsByExt(std::string ext, FormatEntryListByExt** entries);
FormatPlugin* getFormat(AbstractBuffer* buffer);
AssemblerPlugin* getAssembler(const std::string &id);
void setLoggerCallback(const Runtime::LogCallback &logcb);
void setStatusCallback(const Runtime::LogCallback& logcb);
void setProgressCallback(const Runtime::ProgressCallback &pcb);
void init(const std::string &temppath = std::string(), const std::string &searchpath = std::string());

}

#endif // PLUGINS_H
