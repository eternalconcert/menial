#ifndef PYRESPONSE_H
#define PYRESPONSE_H
#include "response.h"
#include "request.h"


class PyResponse: public Response {
    public:
        std::string get();
        std::string getHeader(std::string content);
        std::string getFileName(std::string target);

        PyResponse(Request *request): Response (request) {};

};


#endif
