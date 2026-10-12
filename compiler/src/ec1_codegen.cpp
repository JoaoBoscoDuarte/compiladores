#include "ec1_codegen.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

// Template for each kind of tree node (the result always ends up in %rax):
//
//   Const:  mov $<valor>, %rax
//
//   OpBin:  <codigo do operando esquerdo>
//           push %rax                 # guarda o esquerdo na pilha
//           <codigo do operando direito>
//           mov %rax, %rbx            # direito em RBX
//           pop %rax                  # esquerdo em RAX
//           <op> %rbx, %rax           # RAX = esquerdo <op> direito
//
// Left-then-right evaluation with the right operand in RBX keeps the operand
// order correct for '-' and '/'. Every push has a matching pop, so the stack
// is unchanged after any expression, and only RAX/RBX (and RDX for '/') are used.
void generate_expression_code(const Exp& exp, std::ostream& out) {
    if (const Const* c = dynamic_cast<const Const*>(&exp)) {
        out << "    mov $" << c->valor << ", %rax\n";
        return;
    }

    const OpBin* bin = dynamic_cast<const OpBin*>(&exp);
    if (bin == nullptr) {
        return; // unreachable: Const and OpBin are the only node kinds
    }

    generate_expression_code(*bin->esq, out);
    out << "    push %rax\n";
    generate_expression_code(*bin->dir, out);
    out << "    mov %rax, %rbx\n";
    out << "    pop %rax\n";

    switch (bin->op) {
        case '+': out << "    add %rbx, %rax\n"; break;
        case '-': out << "    sub %rbx, %rax\n"; break;
        case '*': out << "    imul %rbx, %rax\n"; break;
        case '/':
            // idiv divides RDX:RAX by the operand; cqo sign-extends RAX into RDX.
            out << "    cqo\n";
            out << "    idiv %rbx\n";
            break;
    }
}

void generate_program(const Exp& tree, std::ostream& out) {
    out << "#\n";
    out << "# modelo de saida para o compilador\n";
    out << "#\n";
    out << ".section .text\n";
    out << ".globl _start\n";
    out << "_start:\n";
    out << "    # expressao: " << tree.to_string() << "\n";
    generate_expression_code(tree, out);
    out << "    call imprime_num\n";
    out << "    call sair\n";
    out << ".include \"x86-64-linux/runtime.s\"\n";
}

bool generate_assembly_file(const std::string& input_file_path, const Exp& tree,
                            std::string& output_path) {
    // Same convention as compilerci: every .s goes to "asm/", named after the
    // input file with the extension swapped to .s.
    static const std::string output_dir = "asm";
    std::filesystem::create_directories(output_dir);

    std::string base_name = std::filesystem::path(input_file_path).filename().string();
    size_t last_dot = base_name.find_last_of('.');
    if (last_dot != std::string::npos) {
        base_name = base_name.substr(0, last_dot);
    }
    output_path = output_dir + "/" + base_name + ".s";

    std::ofstream out(output_path);
    if (!out.is_open()) {
        std::cerr << "Error: Could not create output file " << output_path << std::endl;
        return false;
    }
    generate_program(tree, out);
    return true;
}
