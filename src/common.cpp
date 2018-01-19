#include <fstream>
#include "common.h"
#include "exceptions.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>


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

LogLevel logLevelByName(std::string levelName) {
    if (levelName == "debug") {
        return DEBUG;
    }
    else if (levelName == "info") {
        return INFO;
    }
    else if (levelName == "warning") {
        return WARNING;
    }
    else if (levelName == "error") {
        return ERROR;
    }
    else if (levelName == "critical") {
        return CRITICAL;
    }
    else {
        return NOTSET;
    }
};
