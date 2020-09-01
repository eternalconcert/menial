#ifndef REDIRECTRESPONSE_H
#define REDIRECTRESPONSE_H
#include <stdio.h>
#include "../config/config.h"
#include "../logger/logger.h"
#include "../response/response.h"
#include "../request/request.h"
#include "../common/common.h"
#include "../exceptions/exceptions.h"
#include "../logger/logger.h"

class RedirectResponse: public Response {
    public:
        std::string headerBase();
        std::string get();
        std::string methodNotAllowed();
        std::string getHeader();
        RedirectResponse(Request *request, Config *config, Logger *logger): Response (request, config, logger) {};

};


#endif
