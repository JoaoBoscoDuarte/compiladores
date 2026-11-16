#include "cmd_semantic.hpp"
#include <unordered_set>

namespace {

// Symbol table: the names declared so far. Only presence matters (every
// variable is a 64-bit integer).
using SymbolTable = std::unordered_set<std::string>;

// Thrown internally to unwind back to check_program() on the first error;
// kept private to this file.
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

void check_bloco(const Bloco& bloco, const SymbolTable& declaradas);

void check_cmd(const Cmd& cmd, const SymbolTable& declaradas) {
    if (const Atrib* atrib = dynamic_cast<const Atrib*>(&cmd)) {
        // Assignment never creates a variable: the target must already exist.
        if (declaradas.find(atrib->nome) == declaradas.end()) {
            throw CheckFailure{{atrib->posicao,
                                "atribuicao a variavel '" + atrib->nome + "' que nao foi declarada"}};
        }
        check_exp(*atrib->exp, declaradas);
        return;
    }
    if (const If* cond = dynamic_cast<const If*>(&cmd)) {
        check_exp(*cond->cond, declaradas);
        check_bloco(cond->entao, declaradas);
        check_bloco(cond->senao, declaradas);
        return;
    }
    if (const While* loop = dynamic_cast<const While*>(&cmd)) {
        check_exp(*loop->cond, declaradas);
        check_bloco(loop->corpo, declaradas);
    }
}

void check_bloco(const Bloco& bloco, const SymbolTable& declaradas) {
    for (const auto& cmd : bloco) {
        check_cmd(*cmd, declaradas);
    }
}

} // namespace

bool check_program(const Programa& programa, SemanticError& error) {
    try {
        SymbolTable declaradas;
        for (const Decl& decl : programa.declaracoes) {
            check_exp(*decl.exp, declaradas);
            declaradas.insert(decl.nome);
        }
        check_bloco(programa.comandos, declaradas);
        check_exp(*programa.exp_final, declaradas);
        return true;
    } catch (const CheckFailure& failure) {
        error = failure.error;
        return false;
    }
}
