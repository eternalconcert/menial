#include <fstream>
#include "common.h"
#include "exceptions.h"
#include <vector>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

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


std::string base64decode(std::string const &encoded) {
    std::string out;
    std::vector<int> T(256, -1);
    for (int i=0; i < 64; i++) {
        T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
    }
    int val = 0;
    int valb = -8;

    for (char c: encoded) {
        if (T[c] == -1) {
            break;
        }
        val = (val << 6) + T[c];
        valb += 6;

        if (valb >= 0) {
            out.push_back(char((val>>valb)&0xFF));
            valb -= 8;
        }
    }
    return out;
}


std::string sha256hash(const std::string input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.size());
    SHA256_Final(hash, &sha256);
    std::stringstream res;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        res << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return res.str();
}
