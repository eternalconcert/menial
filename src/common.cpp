#include <fstream>
#include "common.h"
#include "exceptions.h"


void error(const char *msg)
{
    perror(msg);
    exit(0);
}


std::string readFile(std::string path) {
    std::ifstream file(path);

    if (!file) {
        throw FileNotFoundException();
    }

    try {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return content;
    } catch (std::ios_base::failure) {
        throw FileNotFoundException();
    }
}
