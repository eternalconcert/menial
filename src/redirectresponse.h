#ifndef REDIRECTRESPONSE_H
#define REDIRECTRESPONSE_H
#include "config.h"
#include "logger.h"
#include "response.h"
#include "request.h"


class RedirectResponse: public Response {
    public:
        std::string headerBase();
        std::string get();
        std::string methodNotAllowed();
        std::string getHeader();
        RedirectResponse(Request *request, Config *config, Logger *logger): Response (request, config, logger) {};

};


#endif
