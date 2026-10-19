#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "ec1_ast.hpp"
#include "ec1_codegen.hpp"
#include "ec1_lexer.hpp"
#include "ec1_parser.hpp"

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

static void print_usage(const char* prog) {
    std::cerr << "Uso: " << prog << " [opcoes] <arquivo.ec1>\n"
              << "Opcoes (combinaveis; sem opcoes equivale a --asm):\n"
              << "  --tokens  imprime a sequencia de tokens\n"
              << "  --tree    imprime a arvore sintatica\n"
              << "  --eval    interpreta e imprime o valor da expressao\n"
              << "  --asm     gera o assembly x86-64 em asm/<nome>.s\n";
}

int main(int argc, char const* argv[]) {
    bool opt_tokens = false, opt_tree = false, opt_eval = false, opt_asm = false;
    std::string input_path;

    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--tokens") == 0) opt_tokens = true;
        else if (std::strcmp(argv[i], "--tree") == 0) opt_tree = true;
        else if (std::strcmp(argv[i], "--eval") == 0) opt_eval = true;
        else if (std::strcmp(argv[i], "--asm") == 0) opt_asm = true;
        else if (argv[i][0] == '-' && argv[i][1] == '-') {
            std::cerr << "Error: opcao desconhecida " << argv[i] << std::endl;
            print_usage(argv[0]);
            return 1;
        } else if (input_path.empty()) {
            input_path = argv[i];
        } else {
            std::cerr << "Error: mais de um arquivo especificado" << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }

    if (input_path.empty()) {
        std::cerr << "Error: No file specified" << std::endl;
        print_usage(argv[0]);
        return 1;
    }
    if (!opt_tokens && !opt_tree && !opt_eval && !opt_asm) {
        opt_asm = true;
    }

    std::string source;
    if (!read_file(input_path, source)) {
        return 1;
    }

    // Analise lexica.
    std::vector<Token> tokens;
    LexError lex_error;
    if (!tokenize(source, tokens, lex_error)) {
        std::cerr << "Erro lexico na posicao " << lex_error.posicao
                  << ": caractere inesperado '" << lex_error.caractere << "'" << std::endl;
        return 1;
    }

    if (opt_tokens) {
        std::cout << "-- tokens --" << std::endl;
        for (const Token& tok : tokens) {
            std::cout << "<" << token_type_name(tok.tipo) << ", \"" << tok.lexema
                      << "\", " << tok.posicao << ">" << std::endl;
        }
    }

    // Analise sintatica.
    std::unique_ptr<Exp> tree;
    SyntaxError syntax_error;
    if (!parse(tokens, tree, syntax_error)) {
        std::cerr << "Erro sintatico na posicao " << syntax_error.posicao
                  << ": " << syntax_error.mensagem << std::endl;
        return 1;
    }

    if (opt_tree) {
        std::cout << "-- arvore --" << std::endl;
        std::cout << tree->to_string() << std::endl;
    }

    // Interpretacao.
    if (opt_eval) {
        std::cout << "-- resultado --" << std::endl;
        try {
            std::cout << tree->eval() << std::endl;
        } catch (const std::runtime_error& e) {
            std::cerr << "Erro de execucao: " << e.what() << std::endl;
            return 1;
        }
    }

    // Geracao de codigo.
    if (opt_asm) {
        std::string output_path;
        if (!generate_assembly_file(input_path, *tree, output_path)) {
            return 1;
        }
        std::cout << "Successfully generated: " << output_path << std::endl;
    }

    return 0;
}
