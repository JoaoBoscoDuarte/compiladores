#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include <vector>
#include "ast.hpp"
#include "lexer.hpp"

// Syntax error: token found doesn't fit the grammar at that point.
struct SyntaxError {
    size_t posicao;
    std::string mensagem;
};

// Recursive-descent parser for <expressao> ::= <num> | '(' <exp> <op> <exp> ')'.
// Consumes the whole token stream; returns true on success (tree filled in)
// or false on the first syntax error (error filled in instead).
bool parse(const std::vector<Token>& tokens, std::unique_ptr<Exp>& tree, SyntaxError& error);

#endif
