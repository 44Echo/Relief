#include "TextFile.h" // Include the header file for the ReadTextFile function
#include <fstream> // Include the fstream library for file input/output
#include <sstream> // Include the sstream library for string stream operations

bool readTextFile(const std::string& path, std::string& out){
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    out = buffer.str();
    return true;
}