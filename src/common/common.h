#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

const int BUFFER_SIZE = 256;
const int BUFFER_LIMIT = BUFFER_SIZE - 1;
const char END_OF_MESSAGE = EOF;

void error(const char *msg);

#endif
