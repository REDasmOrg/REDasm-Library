#include "lexer.h"
#include <rdcore/support/lexer.h>

RDLexer* RDLexer_Create(const char* s) { return CPTR(RDLexer, new Lexer(s)); }
size_t RDLexer_Consume(RDLexer* lex, const RDToken** tokens, const char** err) { return CPTR(Lexer, lex)->consume(tokens, err); }
const char* RDLexer_GetLastError(const RDLexer* lex) { return CPTR(const Lexer, lex)->lastError().c_str(); }
bool RDLexer_Lex(RDLexer* lex, RDToken* token) { return CPTR(Lexer, lex)->lex(token); }
bool RDLexer_LexCheck(RDLexer* lex, RDToken* token, rd_type type) { return CPTR(Lexer, lex)->lexCheck(token, type); }
bool RDLexer_Check(const RDLexer* lex, const RDToken* token, rd_type type) { return CPTR(const Lexer, lex)->check(token, type); }
bool RDLexer_HasError(const RDLexer* lex) { return CPTR(const Lexer, lex)->hasError(); }
void RDLexer_Error(RDLexer* lex, const RDToken* token) { CPTR(Lexer, lex)->error(token); }
void RDLexer_Rewind(RDLexer* lex) { CPTR(Lexer, lex)->rewind(); }

bool RDToken_Equals(const RDToken* t1, const RDToken* t2) { return Lexer::equals(t1, t2); }

const char* RDToken_GetValue(const RDToken* t)
{
    static std::string s;
    s = Lexer::tokenValue(t);
    return s.c_str();
}
