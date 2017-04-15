#ifndef CACHE_H
#define CACHE_H

#include <map>
#include <string>

class Cache {
    public:
        std::string get(std::string);
        void set(std::string key, std::string value);
        Cache();

    private:
        std::map<std::string, std::string> storage;
};

#endif
