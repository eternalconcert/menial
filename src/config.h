#ifndef CONFIG_H
#define CONFIG_H
#include <set>
#include <map>
#include <string>
#include "common.h"

class Config {
    public:
        void update(std::string path);
        LogLevel logLevel;
        std::string logFilePath;
        std::string logger;
        std::map<std::string, std::map<std::string, std::string>> hosts;
        std::set<int> ports;
        std::string responder;
        static Config* getConfig();

    private:
        static Config *_instance;
        Config();
};

#endif
