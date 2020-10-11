#pragma once

#include "../macros.h"
#include "../types.h"

DECLARE_HANDLE(RDLexer);

enum RDTokenType {
    TokenType_End,
    TokenType_Unexpected,
    TokenType_Marker,
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

RD_API_EXPORT RDLexer* RDLexer_Create(const char* s);
RD_API_EXPORT size_t RDLexer_Consume(RDLexer* lex, const RDToken** tokens, const char** err);
RD_API_EXPORT const char* RDLexer_GetLastError(const RDLexer* lex);
RD_API_EXPORT bool RDLexer_Lex(RDLexer* lex, RDToken* token);
RD_API_EXPORT bool RDLexer_LexCheck(RDLexer* lex, RDToken* token, rd_type type);
RD_API_EXPORT bool RDLexer_Check(RDLexer* lex, const RDToken* token, rd_type type);
RD_API_EXPORT bool RDLexer_HasError(const RDLexer* lex);
RD_API_EXPORT void RDLexer_Rewind(RDLexer* lex);

RD_API_EXPORT bool RDToken_Equals(const RDToken* t1, const RDToken* t2);
RD_API_EXPORT const char* RDToken_GetValue(const RDToken* t);
