#ifndef CACHE_H
#define CACHE_H

#include <map>
#include <string>
#include "messagehandler.h"


class Cache: public MessageHandler {
    public:
        std::string handleMessage(std::string message);

        std::string get(std::string);
        void set(std::string key, std::string value);
        Cache();

    private:
        std::map<std::string, std::string> storage;
};

#endif
