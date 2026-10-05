#include <iostream>
#include <string>

#include "parser.hpp"
#include "codegen.hpp"

int main(int argc, char const *argv[]) {
    // Checking CLI parameters
    if (argc < 2) {
        std::cerr << "Error: No file specified" << std::endl;
        std::cerr << "Correct usage: " << argv[0] << " <file.ci>" << std::endl;
        return 1;
    }

    std::string file_path = argv[1];
    std::string number;

    // Reading + Syntactic Validation
    if (!parse_file(file_path, number)) {
        return 1;
    }

    // assembly generate
    if (!generate_assembly(file_path, number)) {
        return 1;
    }

    return 0;
}