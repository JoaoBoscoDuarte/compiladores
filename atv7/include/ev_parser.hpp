#ifndef EV_PARSER_HPP
#define EV_PARSER_HPP

#include "ev_ast.hpp"
#include "ev_lexer.hpp"
#include <vector>

// Syntax error: token found doesn't fit the grammar at that point.
struct SyntaxError {
    size_t posicao;
    std::string mensagem;
};

// Recursive-descent parser for EV:
//   <programa> ::= <decl>* '=' <exp>
//   <decl>     ::= <ident> '=' <exp> ';'
//   <exp>      ::= <exp_m> (('+' | '-') <exp_m>)*
//   <exp_m>    ::= <prim>  (('*' | '/') <prim>)*
//   <prim>     ::= <num> | <ident> | '(' <exp> ')'
// Consumes the whole token stream; returns true on success (programa filled
// in) or false on the first syntax error (error filled in instead).
bool parse(const std::vector<Token>& tokens, Programa& programa, SyntaxError& error);

#endif
