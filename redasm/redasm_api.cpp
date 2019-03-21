#include "redasm_api.h"
#include <iostream>

namespace REDasm {

namespace OperandSizes {

std::string size(u32 opsize)
{
    if(opsize == OperandSizes::Byte)
        return "byte";

    if(opsize == OperandSizes::Word)
        return "word";

    if(opsize == OperandSizes::Dword)
        return "dword";

    if(opsize == OperandSizes::Qword)
        return "qword";

    return std::string();
}

} // namespace OperandSizes

static std::list<std::string> split(const std::string &s, char sep)
{
    std::stringstream ss(s);

    std::list<std::string> seglist;
    std::string segment;

    while(std::getline(ss, segment, sep))
        seglist.push_back(segment);

    return seglist;
}

static std::list<std::string> splitPath(const std::string &path) { return split(path, Context::dirSeparator[0]); }

std::string fileName(const std::string &path)
{
    std::list<std::string> items = splitPath(path);

    if(items.empty() || (items.back() == Context::dirSeparator))
        return std::string();

    return items.back();
}

std::string filePath(const std::string &path)
{
    std::list<std::string> items = splitPath(path);

    if(items.empty() || (items.back() == Context::dirSeparator))
        return path;

    items.pop_back();

    std::string newpath;

    for(const std::string& item : items)
        newpath = REDasm::makePath(newpath, item);

    return newpath;
}

std::string fileNameOnly(const std::string &path)
{
    std::string filename = REDasm::fileName(path);
    size_t pos = filename.find_last_of('.');

    if(pos != std::string::npos)
        filename = filename.substr(0, pos);

    return filename;
}

} // namespace REDasm
