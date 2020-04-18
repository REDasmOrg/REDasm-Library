#include "fs.h"
#include <cstring>
#include <fstream>

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

namespace FS {

Path::Path(const std::string& value): value(value) { }
bool Path::empty() const { return value.empty(); }
bool Path::exists() const { return this->isFile() || this->isDir(); }
bool Path::isDir() const { return Path::isDir(value); }
bool Path::isFile() const { return Path::isFile(value); }

std::string Path::path() const
{
    size_t idx = value.find_last_of(Path::sep());
    if(idx == std::string::npos) return std::string();
    return value.substr(0, idx);
}

std::string Path::name() const
{
    size_t idx = value.find_last_of(Path::sep());
    if(idx == std::string::npos) return std::string();
    return value.substr(idx + 1);
}

std::string Path::stem() const
{
    std::string name = this->name();
    if(name.front() == DOT_EXT) return name;

    size_t idx = name.find_last_of(DOT_EXT);
    if(idx == std::string::npos) return name;

    return name.substr(0, idx);
}

std::string Path::ext() const
{
    std::string name = this->name();
    size_t idx = name.find_last_of(DOT_EXT);
    if(idx == std::string::npos) return std::string();
    return name.substr(idx);
}

Path& Path::append(const std::string& rhs)
{
    if(!value.empty()) value += Path::sep();
    value += rhs;
    return *this;
}

std::string Path::join(const std::string& lhs, const std::string& rhs) { return Path(lhs).append(rhs).value; }
bool Path::isFile(const std::string& filepath) { std::ifstream ifs(filepath.c_str()); return ifs.is_open(); }

bool Path::isDir(const std::string& filepath)
{
    DIR* dir = opendir(filepath.c_str());
    bool isdir = dir != nullptr;
    if(dir) closedir(dir);
    return isdir;
}

bool Path::exists(const std::string& filepath) { return Path::isFile(filepath) || Path::isDir(filepath); }

std::string Path::sep()
{
#ifdef _WIN32
    return "\\";
#else
    return "/";
#endif
}

static void dir(const std::string& path, bool recurse, EntryCallback cb)
{
    struct dirent* entry = nullptr;

    DIR* dir = opendir(path.c_str());
    if(!dir) return;

    while((entry = readdir(dir)))
    {
        if(!std::strcmp(entry->d_name, PATH_DOT) || !std::strcmp(entry->d_name, PATH_DOT_DOT))
            continue;

        std::string abspath = Path(path).append(entry->d_name).value;
        size_t itype = Info::Unknown;

        switch(entry->d_type)
        {
            case DT_DIR:
                itype = Info::Directory;
                if(recurse) FS::dir(abspath, true, cb);
                continue;

            case DT_LNK: itype = Info::SymLink; break;
            case DT_REG: itype = Info::File; break;
            default: continue;
        }

        Entry entry = { Path(abspath), itype };
        cb(&entry);
    }

    closedir(dir);
}

char Path::sepChar() { return Path::sep().front(); }
void recurse(const std::string& path, EntryCallback cb) { FS::dir(path, true, cb); }
void dir(const std::string& path, EntryCallback cb) { FS::dir(path, false, cb); }

EntryList dir(const std::string& path)
{
    EntryList entries;

    FS::dir(path, false, [&entries](const Entry* entry) {
        entries.push_back(*entry);
    });

    return entries;
}

EntryList recurse(const std::string& path)
{
    EntryList entries;

    FS::dir(path, true, [&entries](const Entry* entry) {
        entries.push_back(*entry);
    });

    return entries;
}
} // namespace FS
