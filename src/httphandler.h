#ifndef HTTP_H
#define HTTP_H
#include "requesthandler.h"


class HttpHandler: public RequestHandler {
    public:
        std::string handleRequest(std::string message);
        HttpHandler();
};


#endif
