#ifndef FUN_CODEGEN_HPP
#define FUN_CODEGEN_HPP

#include "fun_ast.hpp"
#include <ostream>
#include <string>

// Emits the full program: .bss section with one .lcomm per global
// variable, then .text with _start (global initializers, the main block and
// its return expression), followed by the code of every function and the
// runtime. See fun_codegen.cpp for the translation scheme and the calling
// convention.
void generate_program(const Programa& programa, std::ostream& out);

// Writes the program's assembly to asm/<input-file-basename>.s, creating
// the directory if needed. Returns false (and logs to stderr) on I/O error.
bool generate_assembly_file(const std::string& input_file_path, const Programa& programa,
                             std::string& output_path);

#endif
