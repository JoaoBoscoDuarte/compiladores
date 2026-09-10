#include "parser.hpp"

#include <iostream>
#include <fstream>
#include <cctype>

// Removes spaces, tabs (\t), line breaks (\n) and carriage returns (\r) from the start and end of the string
static std::string trim(const std::string& str) {
    
    // Search from left to right for the index of the first character that is not a space or a line break
    size_t first = str.find_first_not_of(" \t\n\r");
    // If the string contains only spaces or is empty, the function returns ""
    if (first == std::string::npos) return "";

    size_t last = str.find_last_not_of(" \t\n\r");
    // It extracts and returns only the portion of the string containing the relevant text
    return str.substr(first, (last - first + 1));
}

// Performs syntactic analysis, checking whether the cleaned string satisfies the rule <num> ::= <digit>+.
static bool is_valid_number(const std::string& str) {
    // If the file is completely empty, reject the input
    if (str.empty()) return false;
    
    // Returns true if the character is between 0 and 9
    for (char c : str) {
        // Ensures that the character type is converted to an unsigned value before being passed to std::isdigit
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

// Read file
bool parse_file(const std::string& file_path, std::string& output_number) {
    std::string raw_content;
    std::string line;

    // Attempts to open the file. If this fails (e.g. the file does not exist or there is no read permission)
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << file_path << std::endl;
        return false;
    }

    // and successfully open it, reads it line by line and concatenates it into the variable "raw_content"
    while (std::getline(file, line)) {
        raw_content += line + "\n";
    }
    file.close();

    // It applies the trim function to all the text read and saves the result directly to the variable output_number
    output_number = trim(raw_content);

    // Sintactic Validation
    if (!is_valid_number(output_number)) {
        std::cerr << "Syntax Error: '" << output_number 
                  << "' is not a valid integer constant." << std::endl;
        return false;
    }

    return true;
}