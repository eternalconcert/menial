#include <fstream>
#include <algorithm>
#include "common.h"
#include "exceptions.h"
#include <sstream>
#include <iostream>
#include <map>
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
        throw FileNotFoundException(path);
    }

    try {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return content;
    } catch (const std::ios_base::failure &) {
        throw FileNotFoundException(path);
    }
}

LogLevel logLevelByName(std::string levelName) {
    if (levelName == "highlight") {
        return HIGHLIGHT;
    }
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


std::string lower(std::string inStr) {
    std::transform(inStr.begin(), inStr.end(), inStr.begin(), ::tolower);
    return inStr;
}


std::map<std::string, std::string>* mimeTypeMap = 0;


std::string getMimeType(std::string extension, std::string filePath) {
    extension = lower(extension);
    if (mimeTypeMap == 0) {
        mimeTypeMap = new std::map<std::string, std::string>();

        std::istringstream fileContent(readFile(filePath));
        std::string line;
        while (std::getline(fileContent, line)) {
            std::string ext = line.substr(0, line.find_first_of(" "));
            std::string mimeType = line.substr(line.find_last_of(" ") + 1, std::string::npos);

            mimeTypeMap->operator[](ext) = mimeType;
        }
    }

    std::string fileType = mimeTypeMap->operator[](extension);
    if (fileType == "") {
        fileType = "text/html; charset=utf-8;";
    }
    return fileType;
}
