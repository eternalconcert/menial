#ifndef COMMON_H
#define COMMON_H
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <zlib.h>
#include <algorithm>
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
#include "../exceptions/exceptions.h"

enum LogLevel {DEBUG = 10, INFO = 20, WARNING = 30, ERROR = 40, CRITICAL = 50, HIGHLIGHT = 60, NOTSET = 100};
LogLevel logLevelByName(std::string levelName);

const int BUFFER_SIZE = 4096;
const int BUFFER_LIMIT = BUFFER_SIZE - 1;

const int MAX_HEADER_LENGTH = 2000;
const int MAX_MESSAGE_LENGTH = 20000;

std::string readFile(std::string path);
std::string base64decode(std::string const &encoded);
std::string sha256hash(const std::string input);

std::string getMimeType(std::string extension, std::string filePath);
std::string currentDateTime();

std::string compressString(std::string str, std::string compression);

int nthOccurance(const std::string& str, const std::string& findMe, int nth);

#endif
