#include "lexer.h"
#include <rdcore/support/lexer.h>

RDLexer* RDLexer_Create(const char* s) { return CPTR(RDLexer, new Lexer(s)); }
bool RDLexer_Lex(RDLexer* lex, RDToken* token) { return CPTR(Lexer, lex)->lex(token); }
void RDLexer_Rewind(RDLexer* lex) { CPTR(Lexer, lex)->rewind(); }
bool RDToken_Equals(const RDToken* t1, const RDToken* t2) { return Lexer::equals(t1, t2); }
