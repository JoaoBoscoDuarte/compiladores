#ifndef CMD_SEMANTIC_HPP
#define CMD_SEMANTIC_HPP

#include "cmd_ast.hpp"
#include <string>

// Semantic error: a variable was used (read or assigned) without having
// been declared.
struct SemanticError {
    size_t posicao;
    std::string mensagem;
};

// Walks the declarations in order, building up a symbol table of declared
// variable names (a declaration may only use names declared strictly
// before it). Then checks the commands recursively: both sides of an
// assignment, every condition and every nested block must only use
// declared variables. Assignments never add symbols. Returns true if
// everything resolves, false on the first error found (error filled in).
bool check_program(const Programa& programa, SemanticError& error);

#endif
