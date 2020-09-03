#include "lexer.h"
#include <string_view>
#include <cctype>

Lexer::Lexer(const char* s): m_str(s), m_curr(m_str) { }

bool Lexer::lex(RDToken* token)
{
    if(!m_curr) return false;
    while(std::isspace(this->peek())) this->get();

    if(std::isalpha(this->peek())) return this->identifier(token);

    if(std::isdigit(this->peek()))
    {
        switch(this->peek<1>())
        {
            case 'x':
            case 'X': return this->hexnumber(token);
            default: return this->number(token);
        }
    }

    if(std::ispunct(this->peek())) return this->punct(token);
    if(!this->peek()) return this->stop(TokenType_End, token);
    return this->stop(TokenType_Unexpected, token);
}

void Lexer::rewind()
{
    m_curr = m_str;
    m_line = m_col = m_pos = 0;
}

bool Lexer::equals(const RDToken* t1, const RDToken* t2)
{
    if((!t1 || !t2) || (t1->type != t2->type) || (t1->length != t2->length)) return false;
    return std::string_view(t1->value, t1->length) == std::string_view(t2->value, t2->length);
}

void Lexer::tokenize(rd_type tokentype, RDToken* token, const Lexer::AcceptCharacter& acceptcb)
{
    token->type = tokentype;
    token->value = m_curr;
    token->line = m_line;
    token->col = m_col;
    token->pos = m_pos;

    while(acceptcb(this->peek(), token)) this->get();
    token->length = static_cast<size_t>(m_curr - token->value);
}

void Lexer::atomize(rd_type tokentype, RDToken* token)
{
    token->type = tokentype;
    token->value = m_curr++;
    token->length = 1;
    token->line = m_line;
    token->col = m_col;
    token->pos = m_pos;
}

bool Lexer::identifier(RDToken* token) { this->tokenize(TokenType_Identifier, token, [](char c, RDToken*) -> bool { return std::isalnum(c) || (c == '_'); }); return true; }

bool Lexer::hexnumber(RDToken* token)
{
    // Save pointer & position information
    const char* p = m_curr;
    size_t line = m_line, col = m_col, pos = m_pos;
    this->get(); // Eat '0'
    this->get(); // Eat 'x'

    this->tokenize(TokenType_NumberHex, token, [](char c, RDToken*) -> bool { return std::isxdigit(c); });

    // Adjust token
    token->value = p;
    token->length += 2;
    token->line = line;
    token->col = col;
    token->pos = pos;
    return true;
}

bool Lexer::number(RDToken* token)
{
    this->tokenize(TokenType_Number, token, [](char c, RDToken* t) -> bool {
        if(c == '.') {
            if(t->type == TokenType_NumberReal) return false;
            t->type = TokenType_NumberReal;
            return true;
        }

        return std::isdigit(c);
    });

    return true;
}

bool Lexer::punct(RDToken* token)
{
    switch(this->peek())
    {
        case '!':  this->atomize(TokenType_Exclamation, token); break;
        case '\"': this->atomize(TokenType_DoubleQuote, token); break;
        case '#':  this->atomize(TokenType_Hash, token); break;
        case '$':  this->atomize(TokenType_Dollar, token); break;
        case '%':  this->atomize(TokenType_Percent, token); break;
        case '&':  this->atomize(TokenType_Ampersand, token); break;
        case '\'': this->atomize(TokenType_SingleQuote, token); break;
        case '(':  this->atomize(TokenType_OpenRound, token); break;
        case ')':  this->atomize(TokenType_CloseRound, token); break;
        case '*':  this->atomize(TokenType_Asterisk, token); break;
        case '+':  this->atomize(TokenType_Plus, token); break;
        case ',':  this->atomize(TokenType_Comma, token);break;
        case '-':  this->atomize(TokenType_Minus, token);break;
        case '.':  this->atomize(TokenType_Dot, token);break;
        case '/':  this->atomize(TokenType_Slash, token);break;
        case ':':  this->atomize(TokenType_Colon, token);break;
        case ';':  this->atomize(TokenType_Semicolon, token);break;
        case '<':  this->atomize(TokenType_LessThan, token);break;
        case '=':  this->atomize(TokenType_Equal, token);break;
        case '>':  this->atomize(TokenType_GreaterThan, token);break;
        case '?':  this->atomize(TokenType_Question, token);break;
        case '@':  this->atomize(TokenType_At, token);break;
        case '[':  this->atomize(TokenType_OpenSquare, token);break;
        case '\\': this->atomize(TokenType_BackSlash, token);break;
        case ']':  this->atomize(TokenType_CloseSquare, token);break;
        case '^':  this->atomize(TokenType_Circumflex, token);break;
        case '_':  this->atomize(TokenType_Underscore, token);break;
        case '`':  this->atomize(TokenType_BackQuote, token);break;
        case '{':  this->atomize(TokenType_OpenCurly, token);break;
        case '|':  this->atomize(TokenType_Pipe, token);break;
        case '}':  this->atomize(TokenType_CloseCurly, token);break;
        case '~':  this->atomize(TokenType_Tilde, token);break;
        default:   return this->stop(TokenType_Unexpected, token);
    }

    return true;
}

bool Lexer::stop(rd_type type, RDToken* token)
{
    token->type = type;
    token->value = m_curr;
    token->length = 1;
    token->line = m_line;
    token->col = m_col;
    token->pos = m_pos;

    return false;
}

char Lexer::get()
{
    char c = *m_curr++;
    m_pos++;

    if(c == '\n')
    {
        m_line++;
        m_col = 0;
    }
    else
        m_col++;

    return c;
}
