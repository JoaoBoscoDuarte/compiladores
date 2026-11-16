#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "cmd_ast.hpp"
#include "cmd_codegen.hpp"
#include "cmd_lexer.hpp"
#include "cmd_parser.hpp"
#include "cmd_semantic.hpp"

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
    std::cerr << "Uso: " << prog << " [opcoes] <arquivo.cmd>\n"
              << "Opcoes (combinaveis; sem opcoes equivale a --asm):\n"
              << "  --tokens  imprime a sequencia de tokens\n"
              << "  --tree    imprime a arvore sintatica (declaracoes, comandos e expressao de retorno)\n"
              << "  --eval    executa o programa e imprime o valor retornado\n"
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
    Programa programa;
    SyntaxError syntax_error;
    if (!parse(tokens, programa, syntax_error)) {
        std::cerr << "Erro sintatico na posicao " << syntax_error.posicao
                  << ": " << syntax_error.mensagem << std::endl;
        return 1;
    }

    // Analise semantica: toda variavel usada ou atribuida precisa ter sido declarada antes.
    SemanticError semantic_error;
    if (!check_program(programa, semantic_error)) {
        std::cerr << "Erro semantico na posicao " << semantic_error.posicao
                  << ": " << semantic_error.mensagem << std::endl;
        return 1;
    }

    if (opt_tree) {
        std::cout << "-- arvore --" << std::endl;
        for (const Decl& decl : programa.declaracoes) {
            std::cout << decl.nome << " = " << decl.exp->to_string() << ";" << std::endl;
        }
        std::cout << "{" << std::endl;
        for (const auto& cmd : programa.comandos) {
            cmd->print(std::cout, 2);
        }
        std::cout << "  return " << programa.exp_final->to_string() << ";" << std::endl;
        std::cout << "}" << std::endl;
    }

    // Interpretacao.
    if (opt_eval) {
        std::cout << "-- resultado --" << std::endl;
        try {
            Env env;
            for (const Decl& decl : programa.declaracoes) {
                env[decl.nome] = decl.exp->eval(env);
            }
            for (const auto& cmd : programa.comandos) {
                cmd->exec(env);
            }
            std::cout << programa.exp_final->eval(env) << std::endl;
        } catch (const std::runtime_error& e) {
            std::cerr << "Erro de execucao: " << e.what() << std::endl;
            return 1;
        }
    }

    // Geracao de codigo.
    if (opt_asm) {
        std::string output_path;
        if (!generate_assembly_file(input_path, programa, output_path)) {
            return 1;
        }
        std::cout << "Successfully generated: " << output_path << std::endl;
    }

    return 0;
}
