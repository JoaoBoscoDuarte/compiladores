#ifndef CMD_AST_HPP
#define CMD_AST_HPP

#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

// Interpreter environment: variable name -> value.
using Env = std::unordered_map<std::string, long long>;

// Abstract syntax tree for expressions in Cmd: a constant, a binary
// operation, or a reference to a declared variable. There is no boolean
// type: comparisons yield 1 (true) or 0 (false), and any non-zero value
// counts as true in conditions.
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
    char op; // '+', '-', '*', '/', '<', '>' or '=' (the '==' comparison)
    std::unique_ptr<Exp> esq;
    std::unique_ptr<Exp> dir;

    OpBin(char op_, std::unique_ptr<Exp> esq_, std::unique_ptr<Exp> dir_)
        : op(op_), esq(std::move(esq_)), dir(std::move(dir_)) {}

    long long eval(const Env& env) const override {
        long long a = esq->eval(env);
        long long b = dir->eval(env);
        // + - * wrap around on overflow (like the generated machine code)
        // instead of being undefined behaviour in C++.
        using U = unsigned long long;
        switch (op) {
            case '+': return static_cast<long long>(static_cast<U>(a) + static_cast<U>(b));
            case '-': return static_cast<long long>(static_cast<U>(a) - static_cast<U>(b));
            case '*': return static_cast<long long>(static_cast<U>(a) * static_cast<U>(b));
            case '/': return a / b;
            case '<': return a < b;
            case '>': return a > b;
            case '=': return a == b;
        }
        return 0; // unreachable, op is validated by the parser
    }

    std::string to_string() const override {
        std::string simbolo = (op == '=') ? "==" : std::string(1, op);
        return "(" + esq->to_string() + " " + simbolo + " " + dir->to_string() + ")";
    }
};

// Commands: assignment, conditional (with mandatory else) and loop.
class Cmd {
public:
    virtual ~Cmd() = default;
    virtual void exec(Env& env) const = 0;
    // Prints the command as an indented tree (used by --tree).
    virtual void print(std::ostream& out, int indent) const = 0;
};

using Bloco = std::vector<std::unique_ptr<Cmd>>;

// <atrib> ::= <var> '=' <exp> ';'
class Atrib : public Cmd {
public:
    std::string nome;
    size_t posicao; // offset of the variable on the left-hand side
    std::unique_ptr<Exp> exp;
    Atrib(std::string nome_, size_t posicao_, std::unique_ptr<Exp> exp_)
        : nome(std::move(nome_)), posicao(posicao_), exp(std::move(exp_)) {}

    void exec(Env& env) const override { env[nome] = exp->eval(env); }
    void print(std::ostream& out, int indent) const override {
        out << std::string(indent, ' ') << nome << " = " << exp->to_string() << ";\n";
    }
};

// <if> ::= 'if' <exp> '{' <cmd>* '}' 'else' '{' <cmd>* '}'
class If : public Cmd {
public:
    std::unique_ptr<Exp> cond;
    Bloco entao;
    Bloco senao;
    If(std::unique_ptr<Exp> c, Bloco e, Bloco s)
        : cond(std::move(c)), entao(std::move(e)), senao(std::move(s)) {}

    void exec(Env& env) const override {
        for (const auto& c : cond->eval(env) != 0 ? entao : senao) c->exec(env);
    }
    void print(std::ostream& out, int indent) const override {
        std::string pad(indent, ' ');
        out << pad << "if " << cond->to_string() << " {\n";
        for (const auto& c : entao) c->print(out, indent + 2);
        out << pad << "} else {\n";
        for (const auto& c : senao) c->print(out, indent + 2);
        out << pad << "}\n";
    }
};

// <while> ::= 'while' <exp> '{' <cmd>* '}'
class While : public Cmd {
public:
    std::unique_ptr<Exp> cond;
    Bloco corpo;
    While(std::unique_ptr<Exp> c, Bloco b) : cond(std::move(c)), corpo(std::move(b)) {}

    void exec(Env& env) const override {
        while (cond->eval(env) != 0) {
            for (const auto& c : corpo) c->exec(env);
        }
    }
    void print(std::ostream& out, int indent) const override {
        std::string pad(indent, ' ');
        out << pad << "while " << cond->to_string() << " {\n";
        for (const auto& c : corpo) c->print(out, indent + 2);
        out << pad << "}\n";
    }
};

// <decl> ::= <ident> '=' <exp> ';'
struct Decl {
    std::string nome;
    std::unique_ptr<Exp> exp;
};

// <programa> ::= <decl>* '{' <cmd>* 'return' <exp> ';' '}'
struct Programa {
    std::vector<Decl> declaracoes;
    Bloco comandos;
    std::unique_ptr<Exp> exp_final;
};

#endif
