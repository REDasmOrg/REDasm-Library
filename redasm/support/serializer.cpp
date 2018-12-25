#include "serializer.h"
#include "compression.h"
#include <cstring>

namespace REDasm {
namespace Serializer {

std::string& xorify(std::string& s)
{
    u32 len = s.size();

    for(u32 i = 0; i < len; i++)
        s[i] ^= ((len - i) % 0x100u);

    return s;
}

void serializeString(std::fstream& fs, const std::string& s) { fs << s; }
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

void serializeBuffer(std::fstream &fs, const Buffer &b)
{
    Serializer::serializeScalar(fs, b.size(), sizeof(u64));
    fs.write(reinterpret_cast<const char*>(b.data()), b.size());
}

void deserializeBuffer(std::fstream &fs, Buffer &b)
{
    u64 size = 0;
    Serializer::deserializeScalar(fs, &size);

    b.resize(size);
    fs.read(reinterpret_cast<char*>(b.data()), b.size());
}

bool compressBuffer(std::fstream &fs, Buffer &b)
{
    Buffer cb;

    if(!Compression::deflate(b, cb))
        return false;

    Serializer::serializeBuffer(fs, cb);
    return true;
}

bool decompressBuffer(std::fstream &fs, Buffer &b)
{
    Buffer cb;
    Serializer::deserializeBuffer(fs, cb);

    if(!Compression::inflate(cb, b))
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
