#pragma once

#include <rdapi/types.h>

enum RDTokenType {
    TokenType_End,
    TokenType_Unexpected,
    TokenType_Identifier,
    TokenType_Number,
    TokenType_NumberHex,
    TokenType_NumberReal,
    TokenType_OpenRound,
    TokenType_CloseRound,
    TokenType_OpenSquare,
    TokenType_CloseSquare,
    TokenType_OpenCurly,
    TokenType_CloseCurly,
    TokenType_LessThan,
    TokenType_GreaterThan,
    TokenType_Equal,
    TokenType_Exclamation,
    TokenType_Question,
    TokenType_Dollar,
    TokenType_Percent,
    TokenType_Ampersand,
    TokenType_Plus,
    TokenType_Minus,
    TokenType_Asterisk,
    TokenType_BackSlash,
    TokenType_Slash,
    TokenType_Hash,
    TokenType_Dot,
    TokenType_Comma,
    TokenType_Colon,
    TokenType_Semicolon,
    TokenType_Underscore,
    TokenType_Circumflex,
    TokenType_Pipe,
    TokenType_Tilde,
    TokenType_SingleQuote,
    TokenType_DoubleQuote,
    TokenType_BackQuote,
    TokenType_At,
};

typedef struct RDToken {
    rd_type type;
    const char* value;
    size_t length, line, col, pos;
} RDToken;

class Lexer
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
