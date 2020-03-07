#include "filesystem.h"
#include <impl/types/container_impl.h>
#include <fstream>
#include <cstring>
#include <deque>

#ifdef __unix__
    #include <sys/stat.h>
    #include <unistd.h>
    #include <dirent.h>
#elif _WIN32
    #include <impl/libs/dirent/dirent.h>
    #include <kernel32.h>
    #include <user32.h>
#else
    #error "Unsupported Platform"
#endif

#define DOT_EXT      '.'
#define PATH_DOT     "."
#define PATH_DOT_DOT ".."

namespace REDasm {
namespace FS {

Path::Path(const String& value): value(value) { }
bool Path::empty() const { return value.empty(); }
bool Path::exists() const { return this->isFile() || this->isDir(); }
bool Path::isDir() const { return Path::isDir(value); }
bool Path::isFile() const { return Path::isFile(value); }

String Path::path() const
{
    size_t idx = value.lastIndexOf(Path::sep());
    if(idx == REDasm::npos) return String();
    return value.substring(0, idx);
}

String Path::name() const
{
    size_t idx = value.lastIndexOf(Path::sep());
    if(idx == REDasm::npos) return String();
    return value.substring(idx + 1);
}

String Path::stem() const
{
    String name = this->name();
    if(name.first() == DOT_EXT) return name;

    size_t idx = name.lastIndexOf(DOT_EXT);
    if(idx == REDasm::npos) return name;

    return name.substring(0, idx);
}

String Path::ext() const
{
    String name = this->name();
    size_t idx = name.lastIndexOf(DOT_EXT);
    if(idx == REDasm::npos) return String();
    return name.substring(idx);
}

Path& Path::append(const String& rhs)
{
    if(!value.empty()) value += Path::sep();
    value += rhs;
    return *this;
}

String Path::join(const String& lhs, const String& rhs) { return Path(lhs).append(rhs).value; }
bool Path::isFile(const String& filepath) { std::ifstream ifs(filepath.c_str()); return ifs.is_open(); }

bool Path::isDir(const String& filepath)
{
    DIR* dir = opendir(filepath.c_str());
    bool isdir = dir != nullptr;
    if(dir) closedir(dir);
    return isdir;
}

bool Path::exists(const String& filepath) { return Path::isFile(filepath) || Path::isDir(filepath); }

String Path::sep()
{
#ifdef _WIN32
    return "\\";
#else
    return "/";
#endif
}

static void dir(const REDasm::String& path, std::deque<Entry>& entries, bool recurse)
{
    struct dirent* entry = nullptr;

    DIR* dir = opendir(path.c_str());
    if(!dir) return;

    while((entry = readdir(dir)))
    {
        if(!std::strcmp(entry->d_name, PATH_DOT) || !std::strcmp(entry->d_name, PATH_DOT_DOT))
            continue;

        String abspath = Path(path).append(entry->d_name).value;
        size_t itype = Info::Unknown;

        switch(entry->d_type)
        {
            case DT_DIR:
                itype = Info::Directory;
                if(recurse) FS::dir(abspath, entries, true);
                continue;

            case DT_LNK:
                itype = Info::SymLink;
                continue; // NOTE: Must be implemented

            case DT_REG:
                itype = Info::File;
                break;

            default: continue;
        }

        entries.push_back({ Path(abspath), itype });
    }

    closedir(dir);
}

char Path::sepChar() { return Path::sep().first(); }

EntryList* dir(const String& path)
{
    std::deque<Entry> entries;
    FS::dir(path, entries, false);
    return REDasm::list_adapter<Entry>(entries);
}

EntryList* recurse(const String& path)
{
    std::deque<Entry> entries;
    FS::dir(path, entries, true);
    return REDasm::list_adapter<Entry>(entries);
}

} // namespace FS
} // namespace REDasm
