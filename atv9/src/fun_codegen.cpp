#include "fun_codegen.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

namespace {

// Translation scheme. The result of every expression ends up in %rax, and no
// register stays live across a sub-expression (intermediate values go on the
// stack), so a function call can appear anywhere inside an expression.
//
//   Const:  mov $<valor>, %rax
//   Var:    mov var_<nome>, %rax        # global, a symbol in .bss
//           mov <off>(%rbp), %rax       # local or parameter of the function
//
//   OpBin:  <codigo do operando esquerdo>
//           push %rax
//           <codigo do operando direito>
//           mov %rax, %rbx
//           pop %rax
//           <op> %rbx, %rax             # add, sub, imul, or cqo + idiv
//
//   A < B (also '>' with setg, '==' with setz); right operand first:
//           <codigo B>; push %rax; <codigo A>; pop %rbx
//           xor %rcx, %rcx; cmp %rbx, %rax; setl %cl; mov %rcx, %rax
//
//   Call f(a1, ..., an):
//           <codigo an>; push %rax      # arguments pushed last to first
//           ...
//           <codigo a1>; push %rax
//           call fun_f
//           add $8n, %rsp               # caller removes the arguments
//
//   Function with L locals (frame, from low to high addresses):
//           local 0 .. local L-1  | saved %rbp | return address | param 0 .. n-1
//   so local i is at 8*i(%rbp) and parameter j at 8*L + 16 + 8*j(%rbp).
//           fun_f:
//             push %rbp
//             sub $8L, %rsp
//             mov %rsp, %rbp
//             <codigo de cada local, guardado em 8*i(%rbp)>
//             <codigo dos comandos>
//             <codigo da expressao de retorno>   # resultado em %rax
//             add $8L, %rsp
//             pop %rbp
//             ret
//
//   Names of functions and globals get a "fun_" / "var_" prefix so a program
//   identifier can never collide with a label or a runtime symbol (buffer,
//   sair, Lfim0...).
//
//   if / while / assignment: same schemes as the Cmd language (Lfalso<n>,
//   Lfim<n>, Linicio<n> labels, one counter for the whole program).
class Gerador {
public:
    explicit Gerador(std::ostream& out) : out_(out) {}

    void programa(const Programa& programa) {
        out_ << "#\n";
        out_ << "# modelo de saida para o compilador\n";
        out_ << "#\n";

        // One .lcomm per global variable (8 bytes, a 64-bit integer).
        out_ << ".section .bss\n";
        for (const Declaracao& d : programa.declaracoes) {
            if (!d.fun) {
                out_ << "    .lcomm var_" << d.var.nome << ", 8\n";
            }
        }

        out_ << "\n.section .text\n";
        out_ << ".globl _start\n";
        out_ << "_start:\n";

        // Global initializers run in declaration order (they may call
        // functions declared earlier), then the main block.
        for (const Declaracao& d : programa.declaracoes) {
            if (!d.fun) {
                out_ << "    # var " << d.var.nome << " = " << d.var.exp->to_string() << ";\n";
                exp(*d.var.exp);
                out_ << "    mov %rax, var_" << d.var.nome << "\n";
            }
        }
        bloco(programa.comandos);
        out_ << "    # return " << programa.exp_final->to_string() << ";\n";
        exp(*programa.exp_final);
        out_ << "    call imprime_num\n";
        out_ << "    call sair\n";

        for (const Declaracao& d : programa.declaracoes) {
            if (d.fun) funcao(*d.fun);
        }
        out_ << ".include \"x86-64-linux/runtime.s\"\n";
    }

private:
    std::ostream& out_;
    int rotulos_ = 0;
    // Names resolved to a frame slot in the function being generated
    // (name -> offset from %rbp); empty in _start.
    std::unordered_map<std::string, long> offsets_;

    void ler(const std::string& nome) {
        auto it = offsets_.find(nome);
        if (it != offsets_.end()) {
            out_ << "    mov " << it->second << "(%rbp), %rax\n";
        } else {
            out_ << "    mov var_" << nome << ", %rax\n";
        }
    }

    void guardar(const std::string& nome) {
        auto it = offsets_.find(nome);
        if (it != offsets_.end()) {
            out_ << "    mov %rax, " << it->second << "(%rbp)\n";
        } else {
            out_ << "    mov %rax, var_" << nome << "\n";
        }
    }

    void exp(const Exp& e) {
        if (const Const* c = dynamic_cast<const Const*>(&e)) {
            out_ << "    mov $" << c->valor << ", %rax\n";
            return;
        }
        if (const VarRef* ref = dynamic_cast<const VarRef*>(&e)) {
            ler(ref->nome);
            return;
        }
        if (const Call* call = dynamic_cast<const Call*>(&e)) {
            for (size_t i = call->args.size(); i-- > 0;) {
                exp(*call->args[i]);
                out_ << "    push %rax\n";
            }
            out_ << "    call fun_" << call->nome << "\n";
            if (!call->args.empty()) {
                out_ << "    add $" << 8 * call->args.size() << ", %rsp\n";
            }
            return;
        }
        const OpBin* bin = dynamic_cast<const OpBin*>(&e);
        if (bin == nullptr) {
            return; // unreachable: Const, VarRef, Call and OpBin are the only node kinds
        }

        if (bin->op == '<' || bin->op == '>' || bin->op == '=') {
            exp(*bin->dir);
            out_ << "    push %rax\n";
            exp(*bin->esq);
            out_ << "    pop %rbx\n";
            out_ << "    xor %rcx, %rcx\n";
            out_ << "    cmp %rbx, %rax\n";
            out_ << (bin->op == '<' ? "    setl %cl\n" : bin->op == '>' ? "    setg %cl\n" : "    setz %cl\n");
            out_ << "    mov %rcx, %rax\n";
            return;
        }

        exp(*bin->esq);
        out_ << "    push %rax\n";
        exp(*bin->dir);
        out_ << "    mov %rax, %rbx\n";
        out_ << "    pop %rax\n";
        switch (bin->op) {
            case '+': out_ << "    add %rbx, %rax\n"; break;
            case '-': out_ << "    sub %rbx, %rax\n"; break;
            case '*': out_ << "    imul %rbx, %rax\n"; break;
            case '/':
                // idiv divides RDX:RAX by the operand; cqo sign-extends RAX into RDX.
                out_ << "    cqo\n";
                out_ << "    idiv %rbx\n";
                break;
        }
    }

    // if E { C1 } else { C2 }          while E { C }
    //     <codigo E>                   Linicio<n>:
    //     cmp $0, %rax                     <codigo E>
    //     jz Lfalso<n>                     cmp $0, %rax
    //     <codigo C1>                      jz Lfim<n>
    //     jmp Lfim<n>                      <codigo C>
    // Lfalso<n>:                           jmp Linicio<n>
    //     <codigo C2>                  Lfim<n>:
    // Lfim<n>:
    void cmd(const Cmd& c) {
        if (const Atrib* atrib = dynamic_cast<const Atrib*>(&c)) {
            out_ << "    # " << atrib->nome << " = " << atrib->exp->to_string() << ";\n";
            exp(*atrib->exp);
            guardar(atrib->nome);
        } else if (const If* cond = dynamic_cast<const If*>(&c)) {
            int n = rotulos_++;
            out_ << "    # if " << cond->cond->to_string() << "\n";
            exp(*cond->cond);
            out_ << "    cmp $0, %rax\n";
            out_ << "    jz Lfalso" << n << "\n";
            bloco(cond->entao);
            out_ << "    jmp Lfim" << n << "\n";
            out_ << "Lfalso" << n << ":\n";
            bloco(cond->senao);
            out_ << "Lfim" << n << ":\n";
        } else if (const While* loop = dynamic_cast<const While*>(&c)) {
            int n = rotulos_++;
            out_ << "Linicio" << n << ":\n";
            out_ << "    # while " << loop->cond->to_string() << "\n";
            exp(*loop->cond);
            out_ << "    cmp $0, %rax\n";
            out_ << "    jz Lfim" << n << "\n";
            bloco(loop->corpo);
            out_ << "    jmp Linicio" << n << "\n";
            out_ << "Lfim" << n << ":\n";
        }
    }

    void bloco(const Bloco& b) {
        for (const auto& c : b) cmd(*c);
    }

    void funcao(const FunDecl& f) {
        const long locais = static_cast<long>(f.locais.size());
        offsets_.clear();
        for (size_t j = 0; j < f.params.size(); j++) {
            offsets_[f.params[j].nome] = 8 * locais + 16 + 8 * static_cast<long>(j);
        }

        out_ << "\n# fun " << f.nome << "(";
        for (size_t j = 0; j < f.params.size(); j++) out_ << (j > 0 ? ", " : "") << f.params[j].nome;
        out_ << ")\n";
        out_ << "fun_" << f.nome << ":\n";
        out_ << "    push %rbp\n";
        if (locais > 0) out_ << "    sub $" << 8 * locais << ", %rsp\n";
        out_ << "    mov %rsp, %rbp\n";

        // A local only becomes visible after its own initializer, so an
        // initializer sees the parameters, the earlier locals and the
        // globals (a global with the same name as a later local included).
        for (size_t i = 0; i < f.locais.size(); i++) {
            const Decl& d = f.locais[i];
            out_ << "    # var " << d.nome << " = " << d.exp->to_string() << ";\n";
            exp(*d.exp);
            out_ << "    mov %rax, " << 8 * i << "(%rbp)\n";
            offsets_[d.nome] = 8 * static_cast<long>(i);
        }
        bloco(f.comandos);
        out_ << "    # return " << f.retorno->to_string() << ";\n";
        exp(*f.retorno);

        if (locais > 0) out_ << "    add $" << 8 * locais << ", %rsp\n";
        out_ << "    pop %rbp\n";
        out_ << "    ret\n";
        offsets_.clear();
    }
};

} // namespace

void generate_program(const Programa& programa, std::ostream& out) {
    Gerador(out).programa(programa);
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
