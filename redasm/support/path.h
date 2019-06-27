#pragma once

#include "../types/containers/list.h"
#include "../types/string.h"
#include "../macros.h"

namespace REDasm {

class LIBREDASM_API Path
{
    public:
        Path() = delete;
        static char dirSeparatorChar();
        static String dirSeparator();

    public:
        static inline bool extIs(const String& s, const String& ext) { return Path::ext(s) == ext; }
        static String fileName(const String& path);
        static String fileNameOnly(const String& path);
        static String filePath(const String& path);
        static List splitPath(const String &path);
        static String ext(const String& s);
        static bool exists(const String& s);
        static bool mkdir(const String& path);

    public:
        template<typename... T> static bool mkpath(T... args) { return Path::mkdir(Path::create(std::forward<T>(args)...)); }
        template<typename... T> static String create(const String& p, T... args) {
            String path = p;

            for(const Variant& arg : { args...}) {
                if(!path.empty() && (path.last() != Path::dirSeparatorChar()))
                    path += Path::dirSeparator();
                path += arg.toString();
            }

            return path;
        }
};

} // namespace REDasm
