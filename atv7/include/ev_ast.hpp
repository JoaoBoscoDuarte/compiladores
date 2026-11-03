#ifndef EV_AST_HPP
#define EV_AST_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Interpreter environment: variable name -> value, built incrementally as
// declarations are evaluated in order.
using Env = std::unordered_map<std::string, long long>;

// Abstract syntax tree for expressions in EV. Three node kinds: a constant,
// a binary operation, and a reference to a previously-declared variable.
class Exp {
public:
    virtual ~Exp() = default;
    virtual long long eval(const Env& env) const = 0;
    virtual std::string to_string() const = 0;
};

class Const : public Exp {
public:
    long long valor;
    explicit Const(long long v) : valor(v) {}

    long long eval(const Env&) const override { return valor; }
    std::string to_string() const override { return std::to_string(valor); }
};

// A reference to a variable by name. `posicao` is the token's offset in the
// source, kept around for semantic-error messages ("variavel nao declarada").
class VarRef : public Exp {
public:
    std::string nome;
    size_t posicao;
    VarRef(std::string nome_, size_t posicao_) : nome(std::move(nome_)), posicao(posicao_) {}

    long long eval(const Env& env) const override { return env.at(nome); }
    std::string to_string() const override { return nome; }
};

class OpBin : public Exp {
public:
    char op; // '+', '-', '*' or '/'
    std::unique_ptr<Exp> esq;
    std::unique_ptr<Exp> dir;

    OpBin(char op_, std::unique_ptr<Exp> esq_, std::unique_ptr<Exp> dir_)
        : op(op_), esq(std::move(esq_)), dir(std::move(dir_)) {}

    long long eval(const Env& env) const override {
        long long a = esq->eval(env);
        long long b = dir->eval(env);
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': return a / b;
        }
        return 0; // unreachable, op is validated by the parser
    }

    std::string to_string() const override {
        return "(" + esq->to_string() + " " + op + " " + dir->to_string() + ")";
    }
};

// <decl> ::= <ident> '=' <exp> ';'
struct Decl {
    std::string nome;
    std::unique_ptr<Exp> exp;
};

// <programa> ::= <decl>* '=' <exp>
struct Programa {
    std::vector<Decl> declaracoes;
    std::unique_ptr<Exp> exp_final;
};

#endif
