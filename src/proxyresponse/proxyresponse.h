#ifndef PROXYRESPONSE_H
#define PROXYRESPONSE_H
#include <dirent.h>
#include <ctime>
#include <cstring>
#include <regex>
#include <stdio.h>
#include <sys/stat.h>
#include "../config/config.h"
#include "../logger/logger.h"
#include "../response/response.h"
#include "../request/request.h"
#include "../common/common.h"
#include "../exceptions/exceptions.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


class ProxyResponse: public Response {
    public:
        std::string forward();
        std::string get() {
           return this->forward();
        };
        std::string options() {
           return this->forward();
        };
        std::string head() {
           return this->forward();
        };
        std::string post() {
           return this->forward();
        };
        std::string readFromUpstream();
        std::string target;

        ProxyResponse(Request *request, Config *config, Logger *logger): Response (request, config, logger) {
            this->target = getRequest()->getTarget();
        };

    private:
        std::string getContent();
};


#endif
