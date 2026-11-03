#include "ev_parser.hpp"
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

    // <programa> ::= <decl>* '=' <exp>
    Programa parse_programa() {
        Programa programa;
        while (!at_end() && tokens_[pos_].tipo == TokenType::Identificador) {
            programa.declaracoes.push_back(parse_decl());
        }
        expect(TokenType::Igual, "esperava '=' para a expressao final");
        programa.exp_final = parse_exp();
        if (pos_ != tokens_.size()) {
            fail(tokens_[pos_].posicao, "token inesperado apos o fim do programa");
        }
        return programa;
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

    // <decl> ::= <ident> '=' <exp> ';'
    Decl parse_decl() {
        Decl decl;
        decl.nome = tokens_[pos_].lexema;
        pos_++;
        expect(TokenType::Igual, "esperava '=' apos o nome da variavel");
        decl.exp = parse_exp();
        expect(TokenType::PontoVirgula, "esperava ';' apos a declaracao");
        return decl;
    }

    // <exp> ::= <exp_m> (('+' | '-') <exp_m>)*
    std::unique_ptr<Exp> parse_exp() {
        std::unique_ptr<Exp> esq = parse_exp_m();
        while (!at_end() && (tokens_[pos_].tipo == TokenType::Soma ||
                              tokens_[pos_].tipo == TokenType::Sub)) {
            char op = tokens_[pos_].tipo == TokenType::Soma ? '+' : '-';
            pos_++;
            std::unique_ptr<Exp> dir = parse_exp_m();
            esq = std::make_unique<OpBin>(op, std::move(esq), std::move(dir));
        }
        return esq;
    }

    // <exp_m> ::= <prim> (('*' | '/') <prim>)*
    std::unique_ptr<Exp> parse_exp_m() {
        std::unique_ptr<Exp> esq = parse_prim();
        while (!at_end() && (tokens_[pos_].tipo == TokenType::Mult ||
                              tokens_[pos_].tipo == TokenType::Div)) {
            char op = tokens_[pos_].tipo == TokenType::Mult ? '*' : '/';
            pos_++;
            std::unique_ptr<Exp> dir = parse_prim();
            esq = std::make_unique<OpBin>(op, std::move(esq), std::move(dir));
        }
        return esq;
    }

    // <prim> ::= <num> | <ident> | '(' <exp> ')'
    std::unique_ptr<Exp> parse_prim() {
        if (at_end()) {
            fail(source_end_pos(), "esperava um numero, identificador ou '(', entrada terminou");
        }
        const Token& tok = tokens_[pos_];

        if (tok.tipo == TokenType::Numero) {
            pos_++;
            try {
                return std::make_unique<Const>(std::stoll(tok.lexema));
            } catch (const std::out_of_range&) {
                fail(tok.posicao, "constante '" + tok.lexema + "' fora do intervalo de 64 bits");
            }
        }

        if (tok.tipo == TokenType::Identificador) {
            pos_++;
            return std::make_unique<VarRef>(tok.lexema, tok.posicao);
        }

        if (tok.tipo == TokenType::ParenEsq) {
            pos_++;
            std::unique_ptr<Exp> exp = parse_exp();
            expect(TokenType::ParenDir, "esperava ')'");
            return exp;
        }

        fail(tok.posicao, "esperava um numero, identificador ou '(', encontrado '" + tok.lexema + "'");
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

bool parse(const std::vector<Token>& tokens, Programa& programa, SyntaxError& error) {
    try {
        Parser parser(tokens);
        programa = parser.parse_programa();
        return true;
    } catch (const ParseFailure& failure) {
        error = failure.error;
        return false;
    }
}
