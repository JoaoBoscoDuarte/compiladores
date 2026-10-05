#include "parser.hpp"
#include <stdexcept>

namespace {

// Thrown internally to unwind back to parse() on the first syntax error;
// kept private to this file.
struct ParseFailure {
    SyntaxError error;
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

    std::unique_ptr<Exp> parse_programa() {
        if (pos_ >= tokens_.size()) {
            fail(source_end_pos(), "entrada vazia, esperava uma expressao");
        }
        std::unique_ptr<Exp> exp = parse_expressao();
        if (pos_ != tokens_.size()) {
            fail(tokens_[pos_].posicao, "token inesperado apos o fim da expressao");
        }
        return exp;
    }

private:
    const std::vector<Token>& tokens_;
    size_t pos_ = 0;

    bool at_end() const { return pos_ >= tokens_.size(); }

    [[noreturn]] void fail(size_t posicao, const std::string& mensagem) {
        throw ParseFailure{{posicao, mensagem}};
    }

    size_t source_end_pos() const {
        return tokens_.empty() ? 0 : tokens_.back().posicao + tokens_.back().lexema.size();
    }

    // <expressao> ::= <num> | '(' <exp> <op> <exp> ')'
    std::unique_ptr<Exp> parse_expressao() {
        if (at_end()) {
            fail(source_end_pos(), "esperava um numero ou '(', entrada terminou");
        }
        const Token& tok = tokens_[pos_];

        if (tok.tipo == TokenType::Numero) {
            pos_++;
            return std::make_unique<Const>(std::stoll(tok.lexema));
        }

        if (tok.tipo == TokenType::ParenEsq) {
            pos_++;
            std::unique_ptr<Exp> esq = parse_expressao();
            char op = parse_operador();
            std::unique_ptr<Exp> dir = parse_expressao();
            expect(TokenType::ParenDir, "esperava ')'");
            return std::make_unique<OpBin>(op, std::move(esq), std::move(dir));
        }

        fail(tok.posicao, "esperava um numero ou '(', encontrado '" + tok.lexema + "'");
    }

    // <op> ::= '+' | '-' | '*' | '/'
    char parse_operador() {
        if (at_end()) {
            fail(source_end_pos(), "esperava um operador, entrada terminou");
        }
        const Token& tok = tokens_[pos_];
        switch (tok.tipo) {
            case TokenType::Soma: pos_++; return '+';
            case TokenType::Sub:  pos_++; return '-';
            case TokenType::Mult: pos_++; return '*';
            case TokenType::Div:  pos_++; return '/';
            default:
                fail(tok.posicao, "esperava um operador, encontrado '" + tok.lexema + "'");
        }
    }

    void expect(TokenType tipo, const std::string& mensagem) {
        if (at_end() || tokens_[pos_].tipo != tipo) {
            size_t posicao = at_end() ? source_end_pos() : tokens_[pos_].posicao;
            fail(posicao, mensagem);
        }
        pos_++;
    }
};

} // namespace

bool parse(const std::vector<Token>& tokens, std::unique_ptr<Exp>& tree, SyntaxError& error) {
    try {
        Parser parser(tokens);
        tree = parser.parse_programa();
        return true;
    } catch (const ParseFailure& failure) {
        error = failure.error;
        return false;
    }
}
