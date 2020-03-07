#pragma once

#include "../types/container.h"
#include "../types/string.h"

namespace REDasm {

namespace FS {

enum Info: size_t { Unknown, Directory, File, SymLink };

struct Path
{
    Path(const String& value);
    bool empty() const;
    String path() const;
    String name() const;
    String stem() const;
    String ext() const;

    Path& append(const String& rhs);

    static String sep();
    static char sepChar();

    String value;
};

struct Entry
{
    Path path;
    size_t info;

    inline const String& value() const { return path.value; }
    inline bool isDir() const { return info == Info::Directory; }
    inline bool isFile() const { return info == Info::File; }
    inline bool isSymLink() const { return info == Info::SymLink; }
};

typedef ListAdapter<Entry> EntryList;

EntryList* recurse(const String& path);
EntryList* dir(const String& path);

} // namespace FS

} // namespace REDasm
