#include "src/Program/file/filemanager.h"

// Read File - Reads a file out to a string, referenced from
// https://badvertex.com/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c.html
std::string FileManager::readFile(std::string filepath) {
    std::string content;
    
    std::fstream f;
    f.open(filepath.c_str(), std::ios::in);

    if (!f.is_open()) {
        std::cerr << "ERROR: Could not read file " << filepath << "\t File does not exist.";
        return "";
    }

    std::string line = "";
    while (!f.eof()) {
        std::getline(f, line);
        content.append(line + "\n");
    }

    f.close();
    return content;
}