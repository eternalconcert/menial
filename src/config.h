#ifndef CONFIG_H
#define CONFIG_H
#include <map>
#include <string>
#include "common.h"

class Config {
    public:
        void update(std::string config);
        int port;
        LogLevel logLevel;
        std::string logFilePath;
        std::string logger;
        std::map<std::string, std::map<std::string, std::string>> hosts;
        static Config* getConfig();

    private:
        static Config *_instance;
        Config();
};

#endif
