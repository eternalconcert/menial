#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H
#include <string>


class RequestHandler {
    public:
        std::string handle(std::string message);
        RequestHandler();
};


#endif
