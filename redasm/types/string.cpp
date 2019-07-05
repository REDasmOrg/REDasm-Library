#include "string.h"
#include <impl/types/string_impl.h>
#include <impl/libs/cereal/cereal.hpp>
#include <impl/libs/cereal/archives/binary.hpp>
#include <impl/libs/cereal/types/string.hpp>
#include "containers/list.h"
#include "base.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>

namespace REDasm {

String::String(): m_pimpl_p(new StringImpl()) { }
String::String(String &&rhs): m_pimpl_p(new StringImpl()) { PIMPL_P(String); p->m_data.swap(rhs.pimpl_p()->m_data); rhs.pimpl_p()->m_data.clear(); }
String::String(const String &rhs): m_pimpl_p(new StringImpl()) { PIMPL_P(String); p->m_data = rhs.pimpl_p()->m_data; }
String::String(const char *rhs, size_t size): m_pimpl_p(new StringImpl()) { PIMPL_P(String); p->m_data = std::string(rhs, size); }
String::String(const char *rhs): m_pimpl_p(new StringImpl()) { PIMPL_P(String); p->m_data = rhs; }
String::String(char rhs): m_pimpl_p(new StringImpl()) { PIMPL_P(String); p->m_data = rhs; }
void String::clear() { PIMPL_P(String); p->m_data.clear(); }
const char *String::c_str() const { PIMPL_P(const String); return p->m_data.data(); }
size_t String::size() const { PIMPL_P(const String); return p->m_data.size(); }
size_t String::indexOf(const String &s, size_t idx) const { PIMPL_P(const String); return p->m_data.find(s.pimpl_p()->m_data, idx); }
size_t String::indexOf(char ch, size_t idx) const { PIMPL_P(const String); return p->m_data.find(ch, idx); }
size_t String::lastIndexOf(const String &s, size_t idx) const { PIMPL_P(const String); return p->m_data.find_last_of(s.pimpl_p()->m_data, idx); }
size_t String::lastIndexOf(char ch, size_t idx) const { PIMPL_P(const String); return p->m_data.find_last_of(ch, idx); }
char String::first() const { PIMPL_P(const String); return p->m_data.front(); }
char String::last() const { PIMPL_P(const String); return p->m_data.back(); }
char& String::first() { PIMPL_P(String); return p->m_data.front(); }
char& String::last() { PIMPL_P(String); return p->m_data.back(); }
bool String::empty() const { PIMPL_P(const String); return p->m_data.empty(); }
bool String::startsWith(const String &rhs) const { PIMPL_P(const String); return !p->m_data.find(rhs.pimpl_p()->m_data); }

bool String::endsWith(const String &rhs) const
{
    PIMPL_P(const String);

    if(p->m_data.size() < rhs.pimpl_p()->m_data.size())
        return false;

    return !p->m_data.compare(p->m_data.size() - rhs.pimpl_p()->m_data.size(),
                              rhs.pimpl_p()->m_data.size(), rhs.pimpl_p()->m_data);
}

bool String::contains(const String &s) const { PIMPL_P(const String); return p->m_data.find(s.pimpl_p()->m_data) != std::string::npos; }
int String::toInt(int base) const { PIMPL_P(const String); return std::stoi(p->m_data, nullptr, base); }

String &String::remove(size_t start, size_t len)
{
    PIMPL_P(String);
    p->m_data.erase(start, len);
    return *this;
}

String& String::remove(char ch)
{
    PIMPL_P(String);
    p->m_data.erase(std::remove(p->m_data.begin(), p->m_data.end(), ch), p->m_data.end());
    return *this;
}

String& String::replace(const String &from, const String &to)
{
    PIMPL_P(String);
    size_t pos = p->m_data.find(from.pimpl_p()->m_data);

    while(pos != std::string::npos)
    {
        p->m_data.replace(pos, from.size(), to.pimpl_p()->m_data);
        pos = p->m_data.find(from.pimpl_p()->m_data, pos + to.size());
    }

    return *this;
}

String &String::replace(char from, char to) { return this->replace(String(from), String(to)); }
String String::substring(size_t pos, size_t len) const { PIMPL_P(const String); return p->m_data.substr(pos, len).c_str(); }

String String::ltrimmed() const
{
    PIMPL_P(const String);
    std::string s = p->m_data;

    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) -> bool {
        return !std::isspace(ch);
    }));

    return s.c_str();
}

String String::rtrimmed() const
{
    PIMPL_P(const String);
    std::string s = p->m_data;

    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) -> bool {
        return !std::isspace(ch);
    }).base(), s.end());

    return s.c_str();
}

String String::trimmed() const
{
    PIMPL_P(const String);
    std::string s = p->m_data;

    // Left
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));

    // Right
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());

    return s.c_str();
}

String String::simplified() const
{
    PIMPL_P(const String);
    std::string s = p->m_data;

    std::replace_if(s.begin(), s.end(), [](char ch) -> bool  {
        return std::isspace(ch);
    }, ' ');

    return s.c_str();
}

String String::quotedSingle() const { PIMPL_P(const String); return ("'" + p->m_data + "'").c_str(); }
String String::quoted() const { PIMPL_P(const String); return ("\"" + p->m_data + "\"").c_str(); }

String String::hex() const
{
    PIMPL_P(const String);
    std::stringstream ss;

    for(size_t i = 0; i < p->m_data.size(); i++)
    {
        ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex <<
              static_cast<size_t>(p->m_data[i]);
    }

    return ss.str().c_str();
}

List String::split(char sep) const
{
    PIMPL_P(const String);

    List parts;
    std::string part;
    std::stringstream ss(p->m_data);

    while(std::getline(ss, part, sep))
    {
        if(part.empty())
            continue;

        parts.append(part.c_str());
    }

    return parts;
}

String &String::operator +=(char rhs) { PIMPL_P(String); p->m_data += rhs; return *this; }
String &String::operator +=(const char *rhs) { PIMPL_P(String); p->m_data += rhs; return *this; }
String &String::operator +=(const String &rhs) { PIMPL_P(String); p->m_data += rhs.pimpl_p()->m_data; return *this; }
String String::operator +=(char rhs) const { PIMPL_P(const String); return (p->m_data + rhs).c_str(); }
String String::operator +=(const char *rhs) const { PIMPL_P(const String); return (p->m_data + rhs).c_str(); }
String String::operator +=(const String &rhs) const { PIMPL_P(const String); return (p->m_data + rhs.pimpl_p()->m_data).c_str(); }
bool String::operator ==(const String &rhs) const { PIMPL_P(const String); return p->m_data == rhs.pimpl_p()->m_data; }
bool String::operator !=(const String &rhs) const { PIMPL_P(const String); return p->m_data != rhs.pimpl_p()->m_data; }
bool String::operator <=(const String &rhs) const { PIMPL_P(const String); return p->m_data <= rhs.pimpl_p()->m_data; }
bool String::operator <(const String &rhs) const { PIMPL_P(const String); return p->m_data < rhs.pimpl_p()->m_data; }
bool String::operator >=(const String &rhs) const { PIMPL_P(const String); return p->m_data >= rhs.pimpl_p()->m_data; }
bool String::operator >(const String &rhs) const { PIMPL_P(const String); return p->m_data > rhs.pimpl_p()->m_data; }
String &String::operator=(const String &rhs) { PIMPL_P(String); p->m_data = rhs.pimpl_p()->m_data; return *this; }
String &String::operator=(const char *rhs) { PIMPL_P(String); p->m_data += rhs; return *this; }
char String::operator[](size_t idx) const { PIMPL_P(const String); return p->m_data[idx]; }
char &String::operator[](size_t idx) { PIMPL_P(String); return p->m_data[idx]; }
void String::save(cereal::BinaryOutputArchive &a) const { PIMPL_P(const String); a(p->m_data);  }
void String::load(cereal::BinaryInputArchive &a) { PIMPL_P(String); a(p->m_data); }

String String::wide(const char *ws, size_t len)
{
    String s;
    const char* p = ws;

    for(size_t i = 0; i < len; i++, p += sizeof(char) * 2)
        s += *p;

    return s;
}

String String::wide(const u8 *ws, size_t len) { return String::wide(reinterpret_cast<const char*>(ws), len); }
String String::wide(const u16 *ws, size_t len) { return String::wide(reinterpret_cast<const char*>(ws), len); }
String String::hexstring(const unsigned char *data, size_t size) { return String::hexstring(reinterpret_cast<const unsigned char*>(data), size); }

String String::hexstring(const char *data, size_t size)
{
    String s;
    std::stringstream ss;

    for(size_t i = 0; i < size; i++, data++)
    {
        ss << std::uppercase << std::setfill('0') <<
              std::setw(2) << std::hex <<
              static_cast<size_t>(*reinterpret_cast<const u8*>(data));
    }

    s.pimpl_p()->m_data = ss.str();
    return s;
}

String String::repeated(char ch, size_t c)
{
    String s;
    s.m_pimpl_p->m_data = std::string(c, ch);
    return s;
}

template<typename T> String String::number(T value) { return std::to_string(value).c_str(); }

template<typename T> String String::hex(T t, size_t bits, bool withprefix)
{
    std::stringstream ss;

    if(withprefix && (t > (std::is_signed<T>::value ? 9 : 9u)))
        ss << "0x";

    ss << std::uppercase << std::hex;

    if(bits > 0)
        ss << std::setfill('0') << std::setw(bits / 4);

    if(std::is_signed<T>::value && t < 0)
        ss << "-" << (~t) + 1;
    else
        ss << t;

    return ss.str().c_str();
}

template String String::number<s8>(s8);
template String String::number<s16>(s16);
template String String::number<s32>(s32);
template String String::number<s64>(s64);
template String String::number<u8>(u8);
template String String::number<u16>(u16);
template String String::number<u32>(u32);
template String String::number<u64>(u64);
template String String::number<float>(float);
template String String::number<double>(double);

template String String::hex<s8>(s8, size_t, bool);
template String String::hex<s16>(s16, size_t, bool);
template String String::hex<s32>(s32, size_t, bool);
template String String::hex<s64>(s64, size_t, bool);
template String String::hex<u8>(u8, size_t, bool);
template String String::hex<u16>(u16, size_t, bool);
template String String::hex<u32>(u32, size_t, bool);
template String String::hex<u64>(u64, size_t, bool);

void to_json(nlohmann::json &j, const String &s) { j = std::string(s.c_str()); }
void from_json(const nlohmann::json &j, String &s) { s = static_cast<std::string>(j).c_str(); }

} // namespace REDasm

REDasm::String operator+(const REDasm::String &lhs, const REDasm::String &rhs) { return lhs.operator+=(rhs); }

REDasm::String operator+(const REDasm::String &lhs, const char *rhs)
{
    REDasm::String res = lhs;
    res += rhs;
    return res;
}

REDasm::String operator+(const char *lhs, const REDasm::String &rhs)
{
    REDasm::String res = lhs;
    return res + rhs;
}

REDasm::String operator+(const REDasm::String &lhs, char rhs) { return lhs + rhs; }
REDasm::String operator+(char lhs, const REDasm::String &rhs) { return lhs + rhs; }

namespace std {

size_t hash<REDasm::String>::operator()(const REDasm::String& s) const noexcept
{
    return std::hash<std::string>()(s.c_str());
}

} // namespace std;
