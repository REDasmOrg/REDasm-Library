#pragma once

#include <fstream>
#include <deque>
#include "../macros.h"

namespace REDasm {

class LIBREDASM_API Path
{
    public:
        typedef std::deque<std::string> StringParts;

    public:
        Path() = delete;
        static char dirSeparatorChar();
        static std::string dirSeparator();

    public:
        static std::string fileName(const std::string& path);
        static std::string fileNameOnly(const std::string& path);
        static std::string filePath(const std::string& path);
        static StringParts splitPath(const std::string &path);
        static std::string ext(const std::string& s);
        static inline bool exists(const std::string& s) { std::ifstream ifs(s); return ifs.is_open(); }
        static inline bool extIs(const std::string& s, const std::string& ext) { return Path::ext(s) == ext; }
        static bool mkdir(const std::string& path);

    public:
        template<typename... T> static bool mkpath(T... args) { return Path::mkdir(Path::create(std::forward<T>(args)...)); }
        template<typename... T> static std::string create(const std::string& p, T... args) {
            std::string path = p;
            std::deque<std::string> v = { args... };

            for(size_t i = 0; i < v.size(); i++) {
                if(!path.empty() && (path.back() != Path::dirSeparatorChar()))
                    path += Path::dirSeparator();
                path += v[i];
            }

            return path;
        }
};

} // namespace REDasm
