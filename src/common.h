#ifndef COMMON_H
#define COMMON_H
#include <string>

enum LogLevel {DEBUG = 10, INFO = 20, WARNING = 30, ERROR = 40, CRITICAL = 50, NOTSET = 100};
LogLevel logLevelByName(std::string levelName);

const int BUFFER_SIZE = 256;
const int BUFFER_LIMIT = BUFFER_SIZE - 1;

void error(const char *msg);
std::string readFile(std::string path);

#endif
