#pragma once

#include "../libs/nlohmann/json_fwd.hpp"
#include "../pimpl.h"
#include "object.h"

namespace REDasm {

class List;
class StringImpl;

class String: public Object
{
    REDASM_OBJECT(String)
    PIMPL_DECLARE_P(String)
    PIMPL_DECLARE_PRIVATE(String)

    public:
        static constexpr size_t npos = size_t(-1);

    public:
        String();
        explicit String(char rhs);
        String(String&& rhs);
        String(const String& rhs);
        String(const char* rhs, size_t size);
        String(const char* rhs);
        void clear();
        const char* c_str() const;
        size_t size() const;
        size_t indexOf(const String& s, size_t idx = 0) const;
        size_t indexOf(char ch, size_t idx = 0) const;
        size_t lastIndexOf(const String& s, size_t idx = REDasm::npos) const;
        size_t lastIndexOf(char ch, size_t idx = REDasm::npos) const;
        char first() const;
        char last() const;
        char& first();
        char& last();
        bool empty() const;
        bool startsWith(const String& rhs) const;
        bool endsWith(const String& rhs) const;
        bool contains(const String& s) const;
        int toInt(int base = 10) const;
        void removeFirst();
        void removeLast();
        String& remove(size_t start, size_t len = npos);
        String& remove(char ch);
        String& replace(const String& from, const String& to);
        String& replace(char from, char to);
        String substring(size_t pos = 0, size_t len = npos) const;
        String left(size_t n = 0) const;
        String right(size_t n = 0) const;
        String ltrimmed() const;
        String rtrimmed() const;
        String trimmed() const;
        String simplified() const;
        String quotedSingle() const;
        String quoted() const;
        String hex() const;
        String capitalized() const;
        String toLower() const;
        String toUpper() const;
        String xorified() const;
        List split(char sep) const;

     public:
        String& operator +=(char rhs);
        String& operator +=(const char* rhs);
        String& operator +=(const String& rhs);
        String operator +=(char rhs) const;
        String operator +=(const char* rhs) const;
        String operator +=(const String& rhs) const;
        bool operator ==(const String& rhs) const;
        bool operator !=(const String& rhs) const;
        bool operator <=(const String& rhs) const;
        bool operator <(const String& rhs) const;
        bool operator >=(const String& rhs) const;
        bool operator >(const String& rhs) const;
        String& operator=(const String& rhs);
        String& operator=(const char* rhs);
        char operator[](size_t idx) const;
        char& operator[](size_t idx);

     public:
        void save(cereal::BinaryOutputArchive& a) const override;
        void load(cereal::BinaryInputArchive& a) override;
        template<typename T> static String number(T value, size_t base = 10, size_t width = 0, char fill = '\0');
        template<typename T> static String hex(T t, size_t bits = 0, bool withprefix = false);
        static String wide(const char* ws, size_t len);
        static String wide(const u8* ws, size_t len);
        static String wide(const u16* ws, size_t len);
        static String hexstring(const unsigned char* data, size_t size);
        static String hexstring(const char* data, size_t size);
        static String repeated(char ch, size_t c);

     friend class RegexImpl;
     friend class RegexMatchIteratorImpl;
};

// JSON support
void to_json(nlohmann::json& j, const String& s);
void from_json(const nlohmann::json& j, String& s);

} // namespace REDasm

REDasm::String operator+(const REDasm::String& lhs, const REDasm::String& rhs);
REDasm::String operator+(const REDasm::String& lhs, const char* rhs);
REDasm::String operator+(const char* lhs, const REDasm::String& rhs);
REDasm::String operator+(const REDasm::String& lhs, char rhs);
REDasm::String operator+(char lhs, const REDasm::String& rhs);

// Hashing support
namespace std {

template<> struct hash<REDasm::String>
{
    size_t operator()(const REDasm::String& s) const noexcept;
};

template<> struct equal_to<REDasm::String>
{
    size_t operator()(const REDasm::String& s1, const REDasm::String& s2) const noexcept;
};

} // namespace std
