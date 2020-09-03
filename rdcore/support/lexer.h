#pragma once

#include <rdapi/support/lexer.h>
#include  "../object.h"

class Lexer: public Object
{
    private:
        typedef bool (*AcceptCharacter)(char c, RDToken* t);

    public:
        Lexer(const char* s);
        bool lex(RDToken* token);
        void rewind();

    public:
        static bool equals(const RDToken* t1, const RDToken* t2);

    private:
        void tokenize(rd_type tokentype, RDToken* token, const AcceptCharacter& acceptcb);
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
        const char *m_str, *m_curr;
        size_t m_line{0}, m_col{0}, m_pos{0};
};
