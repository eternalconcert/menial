#ifndef REDIRECTRESPONSE_H
#define REDIRECTRESPONSE_H
#include <stdio.h>
#include "../config.h"
#include "../logger.h"
#include "../response.h"
#include "../request.h"
#include "../common.h"
#include "../exceptions.h"
#include "../logger.h"

class RedirectResponse: public Response {
    public:
        std::string headerBase();
        std::string get();
        std::string methodNotAllowed();
        std::string getHeader();
        RedirectResponse(Request *request, Config *config, Logger *logger): Response (request, config, logger) {};

};


#endif
