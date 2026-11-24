#include "fun_parser.hpp"
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

    // <programa> ::= <decl>* 'main' '{' <cmd>* 'return' <exp> ';' '}'
    // <decl>     ::= <vardecl> | <fundecl>
    Programa parse_programa() {
        Programa programa;
        while (peek(TokenType::Var) || peek(TokenType::Fun)) {
            Declaracao d;
            if (peek(TokenType::Var)) {
                d.var = parse_vardecl();
            } else {
                d.fun = parse_fundecl();
            }
            programa.declaracoes.push_back(std::move(d));
        }
        expect(TokenType::Main, "esperava 'var', 'fun' ou 'main'");
        expect(TokenType::ChaveEsq, "esperava '{' para iniciar o bloco main");
        programa.comandos = parse_comandos();
        expect(TokenType::Return, "esperava 'return' com a expressao de resultado");
        programa.exp_final = parse_exp();
        expect(TokenType::PontoVirgula, "esperava ';' apos a expressao de resultado");
        expect(TokenType::ChaveDir, "esperava '}' para fechar o bloco main");
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

    const Token& expect_ident(const std::string& mensagem) {
        if (!peek(TokenType::Identificador)) {
            fail(at_end() ? source_end_pos() : tokens_[pos_].posicao, mensagem);
        }
        return tokens_[pos_++];
    }

    // <vardecl> ::= 'var' <id> '=' <exp> ';'
    Decl parse_vardecl() {
        pos_++; // 'var'
        Decl decl;
        const Token& nome = expect_ident("esperava o nome da variavel apos 'var'");
        decl.nome = nome.lexema;
        decl.posicao = nome.posicao;
        expect(TokenType::Igual, "esperava '=' apos o nome da variavel");
        decl.exp = parse_exp();
        expect(TokenType::PontoVirgula, "esperava ';' apos a declaracao");
        return decl;
    }

    // <fundecl> ::= 'fun' <id> '(' <arglist>? ')'
    //               '{' <vardecl>* <cmd>* 'return' <exp> ';' '}'
    std::unique_ptr<FunDecl> parse_fundecl() {
        pos_++; // 'fun'
        auto fun = std::make_unique<FunDecl>();
        const Token& nome = expect_ident("esperava o nome da funcao apos 'fun'");
        fun->nome = nome.lexema;
        fun->posicao = nome.posicao;
        expect(TokenType::ParenEsq, "esperava '(' apos o nome da funcao");
        // <arglist> ::= <id> | <id> ',' <arglist>   (no trailing comma)
        if (!peek(TokenType::ParenDir)) {
            while (true) {
                const Token& p = expect_ident("esperava o nome de um parametro");
                fun->params.push_back({p.lexema, p.posicao});
                if (!peek(TokenType::Virgula)) break;
                pos_++;
            }
        }
        expect(TokenType::ParenDir, "esperava ')' ao fim da lista de parametros");
        expect(TokenType::ChaveEsq, "esperava '{' para iniciar o corpo da funcao");
        while (peek(TokenType::Var)) {
            fun->locais.push_back(parse_vardecl());
        }
        fun->comandos = parse_comandos();
        if (peek(TokenType::Var)) {
            fail(tokens_[pos_].posicao, "declaracoes 'var' devem vir antes dos comandos");
        }
        expect(TokenType::Return, "esperava 'return' com a expressao de resultado da funcao");
        fun->retorno = parse_exp();
        expect(TokenType::PontoVirgula, "esperava ';' apos a expressao de resultado");
        expect(TokenType::ChaveDir, "esperava '}' para fechar o corpo da funcao");
        return fun;
    }

    bool peek(TokenType tipo) const { return !at_end() && tokens_[pos_].tipo == tipo; }

    // <cmd>* : stops at the first token that cannot start a command
    // (the caller decides whether it is valid, e.g. 'return' or '}').
    Bloco parse_comandos() {
        Bloco bloco;
        while (peek(TokenType::If) || peek(TokenType::While) || peek(TokenType::Identificador)) {
            bloco.push_back(parse_cmd());
        }
        return bloco;
    }

    // '{' <cmd>* '}' (the braces are mandatory)
    Bloco parse_bloco() {
        expect(TokenType::ChaveEsq, "esperava '{'");
        Bloco bloco = parse_comandos();
        if (peek(TokenType::Return)) {
            fail(tokens_[pos_].posicao, "'return' nao e um comando: so pode aparecer no fim do programa");
        }
        expect(TokenType::ChaveDir, "esperava '}'");
        return bloco;
    }

    // <cmd> ::= <if> | <while> | <atrib>
    std::unique_ptr<Cmd> parse_cmd() {
        if (peek(TokenType::If)) {
            pos_++;
            std::unique_ptr<Exp> cond = parse_exp();
            Bloco entao = parse_bloco();
            expect(TokenType::Else, "esperava 'else' (o comando if exige o braco else)");
            Bloco senao = parse_bloco();
            return std::make_unique<If>(std::move(cond), std::move(entao), std::move(senao));
        }
        if (peek(TokenType::While)) {
            pos_++;
            std::unique_ptr<Exp> cond = parse_exp();
            Bloco corpo = parse_bloco();
            return std::make_unique<While>(std::move(cond), std::move(corpo));
        }
        // <atrib> ::= <id> '=' <exp> ';'
        const Token& nome = tokens_[pos_];
        pos_++;
        expect(TokenType::Igual, "esperava '=' apos o nome da variavel");
        std::unique_ptr<Exp> exp = parse_exp();
        expect(TokenType::PontoVirgula, "esperava ';' apos a atribuicao");
        return std::make_unique<Atrib>(nome.lexema, nome.posicao, std::move(exp));
    }

    // <exp> ::= <exp_a> (('<' | '>' | '==') <exp_a>)*   (left-associative)
    std::unique_ptr<Exp> parse_exp() {
        std::unique_ptr<Exp> esq = parse_exp_a();
        while (peek(TokenType::Menor) || peek(TokenType::Maior) || peek(TokenType::IgualIgual)) {
            TokenType t = tokens_[pos_].tipo;
            char op = t == TokenType::Menor ? '<' : t == TokenType::Maior ? '>' : '=';
            pos_++;
            std::unique_ptr<Exp> dir = parse_exp_a();
            esq = std::make_unique<OpBin>(op, std::move(esq), std::move(dir));
        }
        return esq;
    }

    // <exp_a> ::= <exp_m> (('+' | '-') <exp_m>)*
    std::unique_ptr<Exp> parse_exp_a() {
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

    // <prim> ::= <num> | <id> | '(' <exp> ')' | <fun>
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
            // An identifier followed by '(' is a call; otherwise a variable.
            if (peek(TokenType::ParenEsq)) {
                pos_++;
                std::vector<std::unique_ptr<Exp>> args;
                // <params> ::= <exp> | <exp> ',' <params>
                if (!peek(TokenType::ParenDir)) {
                    while (true) {
                        args.push_back(parse_exp());
                        if (!peek(TokenType::Virgula)) break;
                        pos_++;
                    }
                }
                expect(TokenType::ParenDir, "esperava ')' ao fim da lista de argumentos");
                return std::make_unique<Call>(tok.lexema, tok.posicao, std::move(args));
            }
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
