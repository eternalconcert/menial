#ifndef HTTP_H
#define HTTP_H

#include <string>
#include "messagehandler.h"


class Request {
    public:
        std::string header;
        std::string body;
        std::string method;
        std::string target;
        std::string host;
        Request(std::string fullMessage);
};

class HttpHandler: public MessageHandler {
    public:
        std::string handleMessage(std::string message);
        HttpHandler();
};


#endif
