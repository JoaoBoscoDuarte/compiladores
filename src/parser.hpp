#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>

// Reads the file, removes spaces and line breaks, and checks that it is a constant integer.
// Returns true if valid and sets “output_number”.
bool parse_file(const std::string& file_path, std::string& output_number);

#endif