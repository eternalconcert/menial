#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include "common.h"

class Config {
    public:
        void update(std::string config);
        int port;
        LogLevel logLevel;
        std::string logFilePath;

        Config();
};

#endif
