#ifndef HTTP_H
#define HTTP_H

#include <map>
#include <string>
#include "messagehandler.h"


class HttpHandler: public MessageHandler {
    public:
        std::string handleMessage(std::string message);
        HttpHandler();

};

#endif
