#ifndef CONFIG_H
#define CONFIG_H
#include <set>
#include <map>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "../exceptions/exceptions.h"
#include "../common/common.h"

class Config {
    public:
        void update(std::string configPath);
        LogLevel logLevel;
        std::string logFilePath;
        std::string logger;
        std::string staticDir;
        std::string resources;
        std::map<std::string, std::map<std::string, std::string>> hosts;
        std::map<std::string, std::string> defaultHosts;
        std::set<int> ports;
        std::map<int, std::map<std::string, std::string>> sslPortmap;
        bool iplogging;
        bool debug;
        static Config* getConfig(std::string path);

    private:
        static Config *_instance;
        Config();
};

#endif
