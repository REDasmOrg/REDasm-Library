#include "utils.h"
#include <algorithm>
#include <codecvt>
#include <thread>
#include "../buffer/view.h"

std::string Utils::hexString(BufferView* view)
{
    std::stringstream ss;

    for(size_t i = 0; i < view->size(); i++)
    {
        ss << std::uppercase << std::setfill('0') <<
              std::setw(2) << std::hex <<
              static_cast<size_t>(view->at(i));
    }

    return ss.str();
}

std::string Utils::toString(const std::u16string& s)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> converter;
    return converter.to_bytes(s);
}

std::string Utils::quoted(const std::u16string& s) { return Utils::quoted(Utils::toString(s)); }
std::string Utils::quotedSingle(const std::u16string& s) { return Utils::quotedSingle(Utils::toString(s)); }
std::string Utils::quoted(const std::string& s) { return "\"" + s + "\""; }
std::string Utils::quotedSingle(const std::string& s) { return "\"" + s + "\""; }
std::string Utils::simplified(const std::u16string& s) { return Utils::simplified(Utils::toString(s)); }

std::string Utils::simplified(std::string s)
{
    std::replace_if(s.begin(), s.end(), [](char ch) -> bool { return std::isspace(ch); }, ' ');
    return s;
}

void Utils::yloop(const std::function<bool()>& cb) { while(cb()) std::this_thread::yield(); }
