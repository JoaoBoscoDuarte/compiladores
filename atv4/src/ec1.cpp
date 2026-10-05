#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"

static bool read_file(const std::string& path, std::string& content) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << path << std::endl;
        return false;
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    return true;
}

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: No file specified" << std::endl;
        std::cerr << "Correct usage: " << argv[0] << " <file.ec1>" << std::endl;
        return 1;
    }

    std::string source;
    if (!read_file(argv[1], source)) {
        return 1;
    }

    // Analise lexica: produz e imprime a sequencia de tokens.
    std::vector<Token> tokens;
    LexError lex_error;
    if (!tokenize(source, tokens, lex_error)) {
        std::cerr << "Erro lexico na posicao " << lex_error.posicao
                   << ": caractere inesperado '" << lex_error.caractere << "'" << std::endl;
        return 1;
    }

    std::cout << "-- tokens --" << std::endl;
    for (const Token& tok : tokens) {
        std::cout << "<" << token_type_name(tok.tipo) << ", \"" << tok.lexema
                   << "\", " << tok.posicao << ">" << std::endl;
    }

    // Analise sintatica: produz e imprime a arvore sintatica.
    std::unique_ptr<Exp> tree;
    SyntaxError syntax_error;
    if (!parse(tokens, tree, syntax_error)) {
        std::cerr << "Erro sintatico na posicao " << syntax_error.posicao
                   << ": " << syntax_error.mensagem << std::endl;
        return 1;
    }

    std::cout << "-- arvore --" << std::endl;
    std::cout << tree->to_string() << std::endl;

    // Interpretacao: avalia a expressao e imprime o resultado.
    std::cout << "-- resultado --" << std::endl;
    std::cout << tree->eval() << std::endl;

    return 0;
}
