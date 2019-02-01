#ifndef PYRESPONSE_H
#define PYRESPONSE_H
#include "config.h"
#include "logger.h"
#include "response.h"
#include "request.h"


class PyResponse: public Response {
    public:
        std::string get();
        std::string getHeader(std::string content);
        PyResponse(Request *request, Config *config, Logger *logger): Response (request, config, logger) {};
};


#endif
