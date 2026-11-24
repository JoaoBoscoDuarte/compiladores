#include "fun_semantic.hpp"
#include <unordered_map>
#include <unordered_set>

namespace {

// Global symbol table entry: a variable, or a function with its number of
// formal parameters.
struct Simbolo {
    bool funcao;
    size_t aridade;
};
using Globais = std::unordered_map<std::string, Simbolo>;
using Locais = std::unordered_set<std::string>;

// Thrown internally to unwind back to check_program() on the first error;
// kept private to this file.
struct CheckFailure {
    SemanticError error;
};

// Where names are resolved: the local table first (null inside main and in
// global initializers), then the global one.
struct Escopo {
    const Globais& globais;
    const Locais* locais;
};

[[noreturn]] void fail(size_t posicao, const std::string& mensagem) {
    throw CheckFailure{{posicao, mensagem}};
}

void check_exp(const Exp& exp, const Escopo& escopo) {
    if (const VarRef* ref = dynamic_cast<const VarRef*>(&exp)) {
        if (escopo.locais != nullptr && escopo.locais->count(ref->nome) > 0) return;
        auto it = escopo.globais.find(ref->nome);
        if (it == escopo.globais.end()) {
            fail(ref->posicao, "variavel '" + ref->nome + "' nao foi declarada");
        }
        if (it->second.funcao) {
            fail(ref->posicao, "'" + ref->nome + "' e uma funcao, nao uma variavel");
        }
        return;
    }
    if (const OpBin* bin = dynamic_cast<const OpBin*>(&exp)) {
        check_exp(*bin->esq, escopo);
        check_exp(*bin->dir, escopo);
        return;
    }
    if (const Call* call = dynamic_cast<const Call*>(&exp)) {
        auto it = escopo.globais.find(call->nome);
        bool local = escopo.locais != nullptr && escopo.locais->count(call->nome) > 0;
        if (local || (it != escopo.globais.end() && !it->second.funcao)) {
            fail(call->posicao, "'" + call->nome + "' e uma variavel, nao uma funcao");
        }
        if (it == escopo.globais.end()) {
            fail(call->posicao, "funcao '" + call->nome + "' nao foi declarada");
        }
        if (call->args.size() != it->second.aridade) {
            fail(call->posicao, "funcao '" + call->nome + "' espera " +
                                    std::to_string(it->second.aridade) + " argumento(s), recebeu " +
                                    std::to_string(call->args.size()));
        }
        for (const auto& arg : call->args) check_exp(*arg, escopo);
        return;
    }
    // Const: nothing to check.
}

void check_bloco(const Bloco& bloco, const Escopo& escopo);

void check_cmd(const Cmd& cmd, const Escopo& escopo) {
    if (const Atrib* atrib = dynamic_cast<const Atrib*>(&cmd)) {
        // Assignment never creates a variable: the target must be a local
        // or a global variable (never a function).
        bool local = escopo.locais != nullptr && escopo.locais->count(atrib->nome) > 0;
        if (!local) {
            auto it = escopo.globais.find(atrib->nome);
            if (it == escopo.globais.end()) {
                fail(atrib->posicao,
                     "atribuicao a variavel '" + atrib->nome + "' que nao foi declarada");
            }
            if (it->second.funcao) {
                fail(atrib->posicao, "'" + atrib->nome + "' e uma funcao: nao pode receber atribuicao");
            }
        }
        check_exp(*atrib->exp, escopo);
        return;
    }
    if (const If* cond = dynamic_cast<const If*>(&cmd)) {
        check_exp(*cond->cond, escopo);
        check_bloco(cond->entao, escopo);
        check_bloco(cond->senao, escopo);
        return;
    }
    if (const While* loop = dynamic_cast<const While*>(&cmd)) {
        check_exp(*loop->cond, escopo);
        check_bloco(loop->corpo, escopo);
    }
}

void check_bloco(const Bloco& bloco, const Escopo& escopo) {
    for (const auto& cmd : bloco) {
        check_cmd(*cmd, escopo);
    }
}

void check_funcao(const FunDecl& fun, const Globais& globais) {
    Locais locais;
    Escopo escopo{globais, &locais};
    for (const Param& p : fun.params) {
        if (!locais.insert(p.nome).second) {
            fail(p.posicao, "parametro '" + p.nome + "' repetido na funcao '" + fun.nome + "'");
        }
    }
    // A local initializer only sees the parameters and the locals declared
    // before it.
    for (const Decl& d : fun.locais) {
        check_exp(*d.exp, escopo);
        if (!locais.insert(d.nome).second) {
            fail(d.posicao, "variavel '" + d.nome + "' ja foi declarada na funcao '" + fun.nome + "'");
        }
    }
    check_bloco(fun.comandos, escopo);
    check_exp(*fun.retorno, escopo);
}

} // namespace

bool check_program(const Programa& programa, SemanticError& error) {
    try {
        Globais globais;
        Escopo global{globais, nullptr};
        for (const Declaracao& d : programa.declaracoes) {
            if (d.fun) {
                const FunDecl& fun = *d.fun;
                if (globais.count(fun.nome) > 0) {
                    fail(fun.posicao, "'" + fun.nome + "' ja foi declarado");
                }
                // Entered before the body so the function can call itself.
                globais[fun.nome] = {true, fun.params.size()};
                check_funcao(fun, globais);
            } else {
                check_exp(*d.var.exp, global);
                if (globais.count(d.var.nome) > 0) {
                    fail(d.var.posicao, "'" + d.var.nome + "' ja foi declarado");
                }
                globais[d.var.nome] = {false, 0};
            }
        }
        check_bloco(programa.comandos, global);
        check_exp(*programa.exp_final, global);
        return true;
    } catch (const CheckFailure& failure) {
        error = failure.error;
        return false;
    }
}
