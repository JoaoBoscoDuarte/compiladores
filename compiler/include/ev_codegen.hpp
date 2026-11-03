#ifndef EV_CODEGEN_HPP
#define EV_CODEGEN_HPP

#include "ev_ast.hpp"
#include <ostream>
#include <string>

// Emits the assembly for a single expression (result always ends up in
// %rax); see ev_codegen.cpp for the translation scheme.
void generate_expression_code(const Exp& exp, std::ostream& out);

// Emits the full program: .bss section with one .lcomm per declared
// variable, then .text with the code for each declaration (storing its
// result into the variable) followed by the final expression.
void generate_program(const Programa& programa, std::ostream& out);

// Writes the program's assembly to asm/<input-file-basename>.s, creating
// the directory if needed. Returns false (and logs to stderr) on I/O error.
bool generate_assembly_file(const std::string& input_file_path, const Programa& programa,
                             std::string& output_path);

#endif
