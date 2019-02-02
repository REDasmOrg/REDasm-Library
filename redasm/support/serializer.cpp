#include "serializer.h"
#include "compression.h"
#include <cstring>
#include <vector>

namespace REDasm {
namespace Serializer {

std::string& xorify(std::string& s)
{
    u32 len = s.size();

    for(u32 i = 0; i < len; i++)
        s[i] ^= ((len - i) % 0x100u);

    return s;
}

void serializeString(std::fstream& fs, const std::string& s) { fs.write(s.c_str(), s.size() + 1); }
void deserializeString(std::fstream& fs, std::string& s) { std::getline(fs, s, '\0'); }

void obfuscateString(std::fstream &fs, std::string s)
{
    xorify(s);
    Serializer::serializeString(fs, s);
}

void deobfuscateString(std::fstream &fs, std::string &s)
{
    Serializer::deserializeString(fs, s);
    xorify(s);
}

void serializeBuffer(std::fstream &fs, const AbstractBuffer *buffer)
{
    Serializer::serializeScalar(fs, buffer->size(), sizeof(u64));
    fs.write(reinterpret_cast<const char*>(buffer->data()), buffer->size());
}

void deserializeBuffer(std::fstream &fs, AbstractBuffer *buffer)
{
    u64 size = 0;
    Serializer::deserializeScalar(fs, &size);

    buffer->resize(size);
    fs.read(reinterpret_cast<char*>(buffer->data()), buffer->size());
}

bool compressBuffer(std::fstream &fs, const AbstractBuffer *buffer)
{
    MemoryBuffer mb;

    if(!Compression::deflate(buffer, &mb))
        return false;

    Serializer::serializeBuffer(fs, &mb);
    return true;
}

bool decompressBuffer(std::fstream &fs, AbstractBuffer *buffer)
{
    MemoryBuffer mb;
    Serializer::deserializeBuffer(fs, &mb);

    if(!Compression::inflate(&mb, buffer))
        return false;

    return true;
}

bool checkSignature(std::fstream &fs, const std::string& signature)
{
    std::vector<char> s(signature.size());
    fs.read(s.data(), signature.size());

    return !std::memcmp(s.data(), signature.data(), s.size());
}

} // namespace Serializer
} // namespace REDasm
