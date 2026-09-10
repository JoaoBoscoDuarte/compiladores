#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include <string>

// Generates the .s file from the constant read
bool generate_assembly(const std::string& input_file_path, const std::string& number);

#endif