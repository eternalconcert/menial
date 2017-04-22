#ifndef COMMON_H
#define COMMON_H
#include <string>

const int BUFFER_SIZE = 256;
const int BUFFER_LIMIT = BUFFER_SIZE - 1;
const char END_OF_MESSAGE = '\n';

void error(const char *msg);
std::string readFile(std::string path);

#endif
