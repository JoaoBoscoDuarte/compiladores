#include "ev_codegen.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_set>

// Template for each kind of tree node (the result always ends up in %rax):
//
//   Const:  mov $<valor>, %rax
//   VarRef: mov <nome>, %rax         # leaf, reads straight from .bss
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

    if (const VarRef* ref = dynamic_cast<const VarRef*>(&exp)) {
        out << "    mov " << ref->nome << ", %rax\n";
        return;
    }

    const OpBin* bin = dynamic_cast<const OpBin*>(&exp);
    if (bin == nullptr) {
        return; // unreachable: Const, VarRef and OpBin are the only node kinds
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

void generate_program(const Programa& programa, std::ostream& out) {
    out << "#\n";
    out << "# modelo de saida para o compilador\n";
    out << "#\n";

    // One .lcomm per declared variable (8 bytes, a 64-bit integer), in
    // declaration order; a name declared more than once only reserves the
    // space once.
    out << ".section .bss\n";
    std::unordered_set<std::string> ja_declaradas;
    for (const Decl& decl : programa.declaracoes) {
        if (ja_declaradas.insert(decl.nome).second) {
            out << "    .lcomm " << decl.nome << ", 8\n";
        }
    }

    out << "\n.section .text\n";
    out << ".globl _start\n";
    out << "_start:\n";

    for (const Decl& decl : programa.declaracoes) {
        out << "    # " << decl.nome << " = " << decl.exp->to_string() << ";\n";
        generate_expression_code(*decl.exp, out);
        out << "    mov %rax, " << decl.nome << "\n";
    }

    out << "    # = " << programa.exp_final->to_string() << "\n";
    generate_expression_code(*programa.exp_final, out);

    out << "    call imprime_num\n";
    out << "    call sair\n";
    out << ".include \"x86-64-linux/runtime.s\"\n";
}

bool generate_assembly_file(const std::string& input_file_path, const Programa& programa,
                             std::string& output_path) {
    // Same convention as earlier activities: every .s goes to "asm/", named
    // after the input file with the extension swapped to .s.
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
    generate_program(programa, out);
    return true;
}
