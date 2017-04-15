#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#include "common.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
