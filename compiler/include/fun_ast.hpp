#ifndef FUN_AST_HPP
#define FUN_AST_HPP

#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

// Interpreter environment: variable name -> value.
using Env = std::unordered_map<std::string, long long>;

struct FunDecl;

// Execution context of the interpreter: the declared functions, the global
// variables and, while running a function body, that call's own locals and
// parameters (which hide globals with the same name).
struct Ctx {
    const std::unordered_map<std::string, const FunDecl*>& funcs;
    Env& globais;
    Env* locais; // nullptr outside functions (global initializers, main)

    long long get(const std::string& nome) const {
        if (locais != nullptr) {
            auto it = locais->find(nome);
            if (it != locais->end()) return it->second;
        }
        return globais.at(nome);
    }
    void set(const std::string& nome, long long valor) {
        if (locais != nullptr) {
            auto it = locais->find(nome);
            if (it != locais->end()) { it->second = valor; return; }
        }
        globais[nome] = valor;
    }
};

// Abstract syntax tree for expressions in Fun: a constant, a binary
// operation, a reference to a variable or a function call. There is no boolean
// type: comparisons yield 1 (true) or 0 (false), and any non-zero value
// counts as true in conditions.
class Exp {
public:
    virtual ~Exp() = default;
    virtual long long eval(Ctx& ctx) const = 0;
    virtual std::string to_string() const = 0;
};

class Const : public Exp {
public:
    long long valor;
    explicit Const(long long v) : valor(v) {}

    long long eval(Ctx&) const override { return valor; }
    std::string to_string() const override { return std::to_string(valor); }
};

// A reference to a variable by name. `posicao` is the token's offset in the
// source, kept around for semantic-error messages ("variavel nao declarada").
class VarRef : public Exp {
public:
    std::string nome;
    size_t posicao;
    VarRef(std::string nome_, size_t posicao_) : nome(std::move(nome_)), posicao(posicao_) {}

    long long eval(Ctx& ctx) const override { return ctx.get(nome); }
    std::string to_string() const override { return nome; }
};

class OpBin : public Exp {
public:
    char op; // '+', '-', '*', '/', '<', '>' or '=' (the '==' comparison)
    std::unique_ptr<Exp> esq;
    std::unique_ptr<Exp> dir;

    OpBin(char op_, std::unique_ptr<Exp> esq_, std::unique_ptr<Exp> dir_)
        : op(op_), esq(std::move(esq_)), dir(std::move(dir_)) {}

    long long eval(Ctx& ctx) const override {
        // The generated code evaluates the right operand of a comparison
        // first (see fun_codegen.cpp); arithmetic goes left to right. This only
        // matters when a call changes a global variable, and keeping the
        // interpreter in the same order keeps it equal to the assembly.
        long long a, b;
        if (op == '<' || op == '>' || op == '=') {
            b = dir->eval(ctx);
            a = esq->eval(ctx);
        } else {
            a = esq->eval(ctx);
            b = dir->eval(ctx);
        }
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

// <fun> ::= <id> '(' <params>? ')'  (a call; defined below, needs FunDecl)
class Call : public Exp {
public:
    std::string nome;
    size_t posicao;
    std::vector<std::unique_ptr<Exp>> args;
    Call(std::string nome_, size_t posicao_, std::vector<std::unique_ptr<Exp>> args_)
        : nome(std::move(nome_)), posicao(posicao_), args(std::move(args_)) {}

    long long eval(Ctx& ctx) const override;
    std::string to_string() const override {
        std::string r = nome + "(";
        for (size_t i = 0; i < args.size(); i++) {
            if (i > 0) r += ", ";
            r += args[i]->to_string();
        }
        return r + ")";
    }
};

// Commands: assignment, conditional (with mandatory else) and loop.
class Cmd {
public:
    virtual ~Cmd() = default;
    virtual void exec(Ctx& ctx) const = 0;
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

    void exec(Ctx& ctx) const override { ctx.set(nome, exp->eval(ctx)); }
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

    void exec(Ctx& ctx) const override {
        for (const auto& c : cond->eval(ctx) != 0 ? entao : senao) c->exec(ctx);
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

    void exec(Ctx& ctx) const override {
        while (cond->eval(ctx) != 0) {
            for (const auto& c : corpo) c->exec(ctx);
        }
    }
    void print(std::ostream& out, int indent) const override {
        std::string pad(indent, ' ');
        out << pad << "while " << cond->to_string() << " {\n";
        for (const auto& c : corpo) c->print(out, indent + 2);
        out << pad << "}\n";
    }
};

// <vardecl> ::= 'var' <id> '=' <exp> ';'  (global or local to a function)
struct Decl {
    std::string nome;
    size_t posicao; // offset of the declared name
    std::unique_ptr<Exp> exp;
};

struct Param {
    std::string nome;
    size_t posicao;
};

// <fundecl> ::= 'fun' <id> '(' <arglist>? ')' '{' <vardecl>* <cmd>* 'return' <exp> ';' '}'
struct FunDecl {
    std::string nome;
    size_t posicao;
    std::vector<Param> params;
    std::vector<Decl> locais;
    Bloco comandos;
    std::unique_ptr<Exp> retorno;

    void print(std::ostream& out) const {
        out << "fun " << nome << "(";
        for (size_t i = 0; i < params.size(); i++) {
            out << (i > 0 ? ", " : "") << params[i].nome;
        }
        out << ") {\n";
        for (const Decl& d : locais) out << "  var " << d.nome << " = " << d.exp->to_string() << ";\n";
        for (const auto& c : comandos) c->print(out, 2);
        out << "  return " << retorno->to_string() << ";\n}\n";
    }
};

// A top-level declaration, in source order: a global variable (fun == null)
// or a function. Order matters: a declaration only sees what came before it.
struct Declaracao {
    Decl var;
    std::unique_ptr<FunDecl> fun;
};

// <programa> ::= <decl>* 'main' '{' <cmd>* 'return' <exp> ';' '}'
struct Programa {
    std::vector<Declaracao> declaracoes;
    Bloco comandos;
    std::unique_ptr<Exp> exp_final;
};

// Call: arguments are evaluated right to left (the order they are pushed on
// the stack by the generated code), then a fresh frame holds the parameters
// and locals of this activation (so recursion works).
inline long long Call::eval(Ctx& ctx) const {
    const FunDecl& f = *ctx.funcs.at(nome);
    std::vector<long long> valores(args.size());
    for (size_t i = args.size(); i-- > 0;) {
        valores[i] = args[i]->eval(ctx);
    }
    Env locais;
    for (size_t i = 0; i < f.params.size(); i++) {
        locais[f.params[i].nome] = valores[i];
    }
    Ctx novo{ctx.funcs, ctx.globais, &locais};
    for (const Decl& d : f.locais) {
        long long v = d.exp->eval(novo);
        locais[d.nome] = v;
    }
    for (const auto& c : f.comandos) c->exec(novo);
    return f.retorno->eval(novo);
}

#endif
