#ifndef EC1_AST_HPP
#define EC1_AST_HPP

#include <memory>
#include <string>

// Abstract syntax tree for EC1. Two node kinds: a constant, and a binary
// operation with a left/right operand (also expressions).
class Exp {
public:
    virtual ~Exp() = default;
    virtual long long eval() const = 0;
    virtual std::string to_string() const = 0;
};

class Const : public Exp {
public:
    long long valor;
    explicit Const(long long v) : valor(v) {}

    long long eval() const override { return valor; }
    std::string to_string() const override { return std::to_string(valor); }
};

class OpBin : public Exp {
public:
    char op; // '+', '-', '*' or '/'
    std::unique_ptr<Exp> esq;
    std::unique_ptr<Exp> dir;

    OpBin(char op_, std::unique_ptr<Exp> esq_, std::unique_ptr<Exp> dir_)
        : op(op_), esq(std::move(esq_)), dir(std::move(dir_)) {}

    long long eval() const override {
        long long a = esq->eval();
        long long b = dir->eval();
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

#endif
