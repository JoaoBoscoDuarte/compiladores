#ifndef CMD_PARSER_HPP
#define CMD_PARSER_HPP

#include "cmd_ast.hpp"
#include "cmd_lexer.hpp"
#include <vector>

// Syntax error: token found doesn't fit the grammar at that point.
struct SyntaxError {
    size_t posicao;
    std::string mensagem;
};

// Recursive-descent parser for Cmd:
//   <programa> ::= <decl>* '{' <cmd>* 'return' <exp> ';' '}'
//   <decl>     ::= <id> '=' <exp> ';'
//   <cmd>      ::= <if> | <while> | <atrib>
//   <if>       ::= 'if' <exp> '{' <cmd>* '}' 'else' '{' <cmd>* '}'
//   <while>    ::= 'while' <exp> '{' <cmd>* '}'
//   <atrib>    ::= <id> '=' <exp> ';'
//   <exp>      ::= <exp_a> (('<' | '>' | '==') <exp_a>)*
//   <exp_a>    ::= <exp_m> (('+' | '-') <exp_m>)*
//   <exp_m>    ::= <prim>  (('*' | '/') <prim>)*
//   <prim>     ::= <num> | <id> | '(' <exp> ')'
// Consumes the whole token stream; returns true on success (programa filled
// in) or false on the first syntax error (error filled in instead).
bool parse(const std::vector<Token>& tokens, Programa& programa, SyntaxError& error);

#endif
