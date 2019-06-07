#include "serializer.h"
#include "compression.h"
#include "../buffer/memorybuffer.h"
#include <cstring>
#include <vector>

namespace REDasm {

std::string& xorify(std::string& s)
{
    size_t len = s.size();

    for(size_t i = 0; i < len; i++)
        s[i] ^= ((len - i) % 0x100u);

    return s;
}

namespace SerializerHelper {

bool signatureIs(std::fstream &fs, const std::string& signature)
{
    std::vector<char> s(signature.size());
    fs.read(s.data(), signature.size());

    return !std::memcmp(s.data(), signature.data(), s.size());
}

void obfuscated(std::fstream &fs, std::string s)
{
    xorify(s);
    Serializer<std::string>::write(fs, s);
}

void deobfuscated(std::fstream &fs, std::string &s)
{
    Serializer<std::string>::read(fs, s);
    xorify(s);
}

bool compressed(std::fstream &fs, const AbstractBuffer *buffer)
{
    MemoryBuffer mb;

    if(!Compression::deflate(buffer, &mb))
        return false;

    Serializer<AbstractBuffer*>::write(fs, &mb);
    return true;
}

bool decompressed(std::fstream &fs, AbstractBuffer *buffer)
{
    MemoryBuffer mb;
    Serializer<AbstractBuffer*>::read(fs, &mb);

    if(!Compression::inflate(&mb, buffer))
        return false;

    return true;
}

} // namespace SerializerHelper

} // namespace REDasm
