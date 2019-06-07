#include "path.h"
#include <sstream>

namespace REDasm {

char Path::dirSeparatorChar() { return Path::dirSeparator()[0]; }

std::string Path::dirSeparator()
{
#ifdef _WIN32
    return "\\";
#else
    return "/";
#endif
}

std::string Path::fileName(const std::string &path)
{
    StringParts parts = Path::splitPath(path);

    if(parts.empty() || (parts.back() == Path::dirSeparator()))
        return std::string();

    return parts.back();
}

std::string Path::fileNameOnly(const std::string &path)
{
    std::string filename = Path::fileName(path);
    size_t pos = filename.find_last_of('.');

    if(pos != std::string::npos)
        filename = filename.substr(0, pos);

    return filename;
}

std::string Path::filePath(const std::string &path)
{
    StringParts parts = Path::splitPath(path);

    if(parts.empty() || (parts.back() == Path::dirSeparator()))
        return path;

    parts.pop_back();
    std::string newpath;

    for(const std::string& part : parts)
        newpath = Path::create(newpath, part);

    return newpath;
}

REDasm::Path::StringParts Path::splitPath(const std::string &path)
{
    std::stringstream ss(path);
    std::deque<std::string> seglist;
    std::string seg;

    while(std::getline(ss, seg, Path::dirSeparatorChar()))
        seglist.push_back(seg);

    return seglist;
}

std::string Path::ext(const std::string &s)
{
    size_t lastidx = s.find_last_of('.');

    if(lastidx == std::string::npos)
        return std::string();

    lastidx++; // Skip '.'

    if(lastidx == std::string::npos)
        return std::string();

    return s.substr(lastidx);
}


} // namespace REDasm
