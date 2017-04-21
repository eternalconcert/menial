#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "exceptions.h"
#include "common.h"


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
