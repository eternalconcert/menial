#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <string>

class MessageHandler {
    public:
        virtual std::string handleMessage(std::string message) { return ""; };
        virtual ~MessageHandler() {};
};

#endif
