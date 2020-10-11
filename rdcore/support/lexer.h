#pragma once

#include <rdapi/support/lexer.h>
#include <string>
#include <vector>
#include "../object.h"

class Lexer: public Object
{
    private:
        typedef bool (*AcceptCharacter)(char c, RDToken* t);

    public:
        Lexer(const char* s);
        size_t consume(const RDToken** tokens, const char** err);
        const std::string& lastError() const;
        bool lex(RDToken* token);
        bool lexCheck(RDToken* token, rd_type type);
        bool check(const RDToken* token, rd_type type) const;
        bool hasError() const;
        void rewind();

    public:
        static bool equals(const RDToken* t1, const RDToken* t2);
        static std::string tokenValue(const RDToken* t);

    private:
        std::string unexpected(const RDToken* token) const;
        void error(const RDToken* token) const;
        void tokenize(rd_type tokentype, RDToken* token, const AcceptCharacter& acceptcb);
        void marker(RDToken* token);
        void atomize(rd_type tokentype, RDToken* token);
        bool identifier(RDToken* token);
        bool hexnumber(RDToken* token);
        bool number(RDToken* token);
        bool punct(RDToken* token);
        bool stop(rd_type type, RDToken* token);
        char get();

    private:
        template<int off = 0> char peek() const { return *(m_curr + off); }

    private:
        mutable std::string m_error;
        std::vector<RDToken> m_tokens;

    private:
        const char *m_str, *m_curr;
        size_t m_line{0}, m_col{0}, m_pos{0};
};
