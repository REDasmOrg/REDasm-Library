#pragma once

#include <string>

class REDasmError
{
    public:
        REDasmError(const std::string& msg, size_t location);
        REDasmError(const std::string& msg);

    private:
        void notifyAndThrow();

    private:
        static std::string m_message;
};
