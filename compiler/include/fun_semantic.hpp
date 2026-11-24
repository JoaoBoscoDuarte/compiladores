#ifndef FUN_SEMANTIC_HPP
#define FUN_SEMANTIC_HPP

#include "fun_ast.hpp"
#include <string>

// Semantic error: undeclared name, name used as the wrong kind of entity
// (variable vs function), wrong number of arguments or a redeclaration.
struct SemanticError {
    size_t posicao;
    std::string mensagem;
};

// Walks the declarations in source order with a global symbol table
// (name -> variable, or function with its arity). A function is entered in
// the table before its body is analyzed, so direct recursion is accepted;
// referring to a function declared later (mutual recursion) is rejected.
// Each function body is checked with a local table (parameters + local
// variables, which hide globals with the same name) in front of the global
// one. Returns true if everything resolves, false on the first error found
// (error filled in).
bool check_program(const Programa& programa, SemanticError& error);

#endif
