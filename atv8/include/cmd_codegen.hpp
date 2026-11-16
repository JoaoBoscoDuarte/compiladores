#ifndef CMD_CODEGEN_HPP
#define CMD_CODEGEN_HPP

#include "cmd_ast.hpp"
#include <ostream>
#include <string>

// Emits the assembly for a single expression (result always ends up in
// %rax); see cmd_codegen.cpp for the translation scheme.
void generate_expression_code(const Exp& exp, std::ostream& out);

// Emits the full program: .bss section with one .lcomm per declared
// variable, then .text with the declarations, the commands (if/while with
// jumps and labels) and the return expression.
void generate_program(const Programa& programa, std::ostream& out);

// Writes the program's assembly to asm/<input-file-basename>.s, creating
// the directory if needed. Returns false (and logs to stderr) on I/O error.
bool generate_assembly_file(const std::string& input_file_path, const Programa& programa,
                             std::string& output_path);

#endif
