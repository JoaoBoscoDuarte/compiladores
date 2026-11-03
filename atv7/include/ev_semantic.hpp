#ifndef EV_SEMANTIC_HPP
#define EV_SEMANTIC_HPP

#include "ev_ast.hpp"
#include <string>

// Semantic error: a variable was referenced before being declared.
struct SemanticError {
    size_t posicao;
    std::string mensagem;
};

// Walks the declarations in order, building up a symbol table of declared
// variable names. For each declaration, the right-hand expression may only
// reference names declared strictly before it; then its own name is added.
// The final expression is checked against the full symbol table. Returns
// true if every variable reference resolves, false on the first undeclared
// reference found (error filled in instead).
bool check_program(const Programa& programa, SemanticError& error);

#endif
