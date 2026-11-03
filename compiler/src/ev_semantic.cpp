#include "ev_semantic.hpp"
#include <stdexcept>
#include <unordered_set>

namespace {

// Symbol table: names declared so far, in a fresh compiler this only needs
// to track presence (see docs/CC-Atividade07, section 5.1).
using SymbolTable = std::unordered_set<std::string>;

// Thrown internally to unwind back to check_program() on the first
// undeclared-variable reference; kept private to this file.
struct CheckFailure {
    SemanticError error;
};

void check_exp(const Exp& exp, const SymbolTable& declaradas) {
    if (const VarRef* ref = dynamic_cast<const VarRef*>(&exp)) {
        if (declaradas.find(ref->nome) == declaradas.end()) {
            throw CheckFailure{{ref->posicao, "variavel '" + ref->nome + "' nao foi declarada"}};
        }
        return;
    }
    if (const OpBin* bin = dynamic_cast<const OpBin*>(&exp)) {
        check_exp(*bin->esq, declaradas);
        check_exp(*bin->dir, declaradas);
        return;
    }
    // Const: nothing to check.
}

} // namespace

bool check_program(const Programa& programa, SemanticError& error) {
    try {
        SymbolTable declaradas;
        for (const Decl& decl : programa.declaracoes) {
            check_exp(*decl.exp, declaradas);
            declaradas.insert(decl.nome);
        }
        check_exp(*programa.exp_final, declaradas);
        return true;
    } catch (const CheckFailure& failure) {
        error = failure.error;
        return false;
    }
}
