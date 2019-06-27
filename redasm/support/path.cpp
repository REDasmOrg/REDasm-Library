#include "path.h"
#include <sstream>
#include <fstream>

#ifdef __unix__
    #include <sys/stat.h>
    #include <unistd.h>
#elif _WIN32
    #include <kernel32.h>
    #include <user32.h>
#else
    #error "Unsupported Platform"
#endif

namespace REDasm {

char Path::dirSeparatorChar() { return Path::dirSeparator()[0]; }

String Path::dirSeparator()
{
#ifdef _WIN32
    return "\\";
#else
    return "/";
#endif
}

String Path::fileName(const String &path)
{
    List parts = Path::splitPath(path);

    if(parts.empty() || (parts.last() == Path::dirSeparator()))
        return String();

    return parts.last().toString();
}

String Path::fileNameOnly(const String &path)
{
    String filename = Path::fileName(path);
    size_t pos = filename.lastIndexOf('.');

    if(pos != String::npos)
        filename = filename.substring(0, pos);

    return filename;
}

String Path::filePath(const String &path)
{
    List parts = Path::splitPath(path);

    if(parts.empty() || (parts.last() == Path::dirSeparator()))
        return path;

    parts.removeLast();
    String newpath;

    auto it = parts.iterator();

    while(it.hasNext())
        newpath = Path::create(newpath, it.next().toString());

    return newpath;
}

List Path::splitPath(const String &path) { return path.split(Path::dirSeparatorChar()); }

String Path::ext(const String &s)
{
    size_t lastidx = s.lastIndexOf('.');

    if(lastidx == String::npos)
        return String();

    lastidx++; // Skip '.'

    if(lastidx == String::npos)
        return String();

    return s.substring(lastidx);
}

bool Path::exists(const String &s) { std::ifstream ifs(s.c_str()); return ifs.is_open(); }

bool Path::mkdir(const String &path)
{
    List folders = Path::splitPath(path);
    String respath;
    bool res = false;

    auto it = folders.iterator();

    while(it.hasNext())
    {
        String folder = it.next().toString();
        respath += folder + Path::dirSeparator();

#ifdef __unix__
        res = ::mkdir(respath.c_str(), S_IRWXU) == 0;
#elif _WIN32
        res = CreateDirectory(path.c_str(), nullptr) != 0;
#endif
    }

    return res;
}


} // namespace REDasm
