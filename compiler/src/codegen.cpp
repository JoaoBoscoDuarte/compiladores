#include "codegen.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

// Signature and Determination of the Exit Path
bool generate_assembly(const std::string& input_file_path, const std::string& number) {
    // All generated .s files go into a single "asm/" output folder,
    // named after the input file (extension swapped .ci -> .s).
    static const std::string output_dir = "asm";
    std::filesystem::create_directories(output_dir);

    std::string base_name = std::filesystem::path(input_file_path).filename().string();
    size_t last_dot = base_name.find_last_of('.');
    if (last_dot != std::string::npos) {
        base_name = base_name.substr(0, last_dot);
    }
    std::string output_path = output_dir + "/" + base_name + ".s";

    // Open a write channel for the new .s file.
    std::ofstream out(output_path);
    if (!out.is_open()) {
        std::cerr << "Error: Could not create output file " << output_path << std::endl;
        return false;
    }

    // mov $<number>, %rax: This is the only line generated dynamically by the compiler. 
    // It loads the numeric value read from the .ci file into the main register %rax.
    out << "#\n";
    out << "# modelo de saida para o compilador\n";
    out << "#\n";
    out << ".section .text\n";
    out << ".globl _start\n";
    out << "_start:\n";
    out << "    mov $" << number << ", %rax\n";
    out << "    call imprime_num\n";
    out << "    call sair\n";
    out << ".include \"x86-64-linux/runtime.s\"\n";

    // Close file
    out.close();
    std::cout << "Successfully generated: " << output_path << std::endl;
    return true;
}