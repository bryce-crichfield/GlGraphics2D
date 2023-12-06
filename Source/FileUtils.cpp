#include <FileUtils.h>

#include <fstream>
#include <iostream>

std::string FileUtils::readTextFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "FileUtils::readTextFile: " << filename << " not found" << std::endl;
        return "";
    }

    std::string line;
    std::string text;
    while (std::getline(file, line)) {
        text += line + "\n";
    }

    return text;
}