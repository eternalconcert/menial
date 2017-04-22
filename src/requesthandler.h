#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H
#include <string>


class RequestHandler {
    public:
        virtual std::string handleRequest(std::string message) { return ""; };
        virtual ~RequestHandler() {};
};


#endif
