#ifndef EC1_CODEGEN_HPP
#define EC1_CODEGEN_HPP

#include <ostream>
#include <string>
#include "ec1_ast.hpp"

// Emits the x86-64 (AT&T syntax) code for an expression. The generated code
// leaves the value in %rax and leaves the stack exactly as it found it.
void generate_expression_code(const Exp& exp, std::ostream& out);

// Emits the complete assembly file (header, _start, expression code,
// calls to imprime_num/sair and the runtime include).
void generate_program(const Exp& tree, std::ostream& out);

// Writes the complete assembly to "asm/<input base name>.s".
// Returns true on success and sets output_path.
bool generate_assembly_file(const std::string& input_file_path, const Exp& tree,
                            std::string& output_path);

#endif
