#ifndef EC1_LEXER_HPP
#define EC1_LEXER_HPP

#include <string>
#include <vector>

// Token classes for the EC1 language.
enum class TokenType {
    Numero,
    ParenEsq,
    ParenDir,
    Soma,
    Sub,
    Mult,
    Div
};

struct Token {
    TokenType tipo;
    std::string lexema;
    size_t posicao; // offset of the first character in the source
};

std::string token_type_name(TokenType tipo);

// Lexical error: an unrecognized character was found at a given position.
struct LexError {
    size_t posicao;
    char caractere;
};

// Scans the whole source and produces the token sequence.
// Returns true on success (tokens filled in); false on the first lexical
// error found (error filled in instead).
bool tokenize(const std::string& source, std::vector<Token>& tokens, LexError& error);

#endif
