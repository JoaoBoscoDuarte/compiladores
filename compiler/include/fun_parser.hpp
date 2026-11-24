#ifndef FUN_PARSER_HPP
#define FUN_PARSER_HPP

#include "fun_ast.hpp"
#include "fun_lexer.hpp"
#include <vector>

// Syntax error: token found doesn't fit the grammar at that point.
struct SyntaxError {
    size_t posicao;
    std::string mensagem;
};

// Recursive-descent parser for Fun:
//   <programa> ::= <decl>* 'main' '{' <cmd>* 'return' <exp> ';' '}'
//   <decl>     ::= <vardecl> | <fundecl>
//   <fundecl>  ::= 'fun' <id> '(' <arglist>? ')'
//                  '{' <vardecl>* <cmd>* 'return' <exp> ';' '}'
//   <arglist>  ::= <id> | <id> ',' <arglist>
//   <vardecl>  ::= 'var' <id> '=' <exp> ';'
//   <cmd>      ::= <if> | <while> | <atrib>
//   <if>       ::= 'if' <exp> '{' <cmd>* '}' 'else' '{' <cmd>* '}'
//   <while>    ::= 'while' <exp> '{' <cmd>* '}'
//   <atrib>    ::= <id> '=' <exp> ';'
//   <exp>      ::= <exp_a> (('<' | '>' | '==') <exp_a>)*
//   <exp_a>    ::= <exp_m> (('+' | '-') <exp_m>)*
//   <exp_m>    ::= <prim>  (('*' | '/') <prim>)*
//   <prim>     ::= <num> | <id> | '(' <exp> ')' | <fun>
//   <fun>      ::= <id> '(' <params>? ')'
//   <params>   ::= <exp> | <exp> ',' <params>
// Consumes the whole token stream; returns true on success (programa filled
// in) or false on the first syntax error (error filled in instead).
bool parse(const std::vector<Token>& tokens, Programa& programa, SyntaxError& error);

#endif
