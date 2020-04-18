#pragma once

#include <functional>
#include <string>
#include <deque>

namespace FS {

enum Info: size_t { Unknown, Directory, File, SymLink };

struct Path
{
    Path(const std::string& value);
    bool empty() const;
    bool exists() const;
    bool isDir() const;
    bool isFile() const;
    std::string path() const;
    std::string name() const;
    std::string stem() const;
    std::string ext() const;

    Path& append(const std::string& rhs);

    static std::string join(const std::string& lhs, const std::string& rhs);
    static bool isFile(const std::string& filepath);
    static bool isDir(const std::string& filepath);
    static bool exists(const std::string& filepath);

    static std::string sep();
    static char sepChar();

    std::string value;
};

struct Entry
{
    Path path;
    size_t info;

    inline const std::string& value() const { return path.value; }
    inline bool isDir() const { return info == Info::Directory; }
    inline bool isFile() const { return info == Info::File; }
    inline bool isSymLink() const { return info == Info::SymLink; }
};

typedef std::deque<Entry> EntryList;
typedef std::function<void(const Entry*)> EntryCallback;

void recurse(const std::string& path, EntryCallback cb);
void dir(const std::string& path, EntryCallback cb);
EntryList recurse(const std::string& path);
EntryList dir(const std::string& path);

} // namespace FS
