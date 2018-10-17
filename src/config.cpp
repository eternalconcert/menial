#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "config.h"
#include "exceptions.h"

using namespace rapidjson;


void Config::update(std::string configPath) {
    std::string config = readFile(configPath);

    Document document;
    ParseResult result = document.Parse(config.c_str());
    if (!result) {
        throw ConfigException("Could not parse config.");
    }

    // logLevel
    Value& logLevel = document["loglevel"];
    if (!logLevel.IsString()) {
        throw ConfigException("No valid config value for loglevel defined.");
    }
    LogLevel level = logLevelByName(logLevel.GetString());
    this->logLevel = level;

    // logFilePath
    Value& logFilePath = document["logfilepath"];
    if (!logFilePath.IsString()) {
        throw ConfigException("No valid config value for logfilepath defined.");
    }

    this->logFilePath = logFilePath.GetString();

    // logger
    Value& logger = document["logger"];
    if (!logger.IsString()) {
        throw ConfigException("No valid config value for logger defined.");
    }
    this->logger = logger.GetString();

    // hosts
    Value& hosts = document["hosts"];
    if (!hosts.IsObject()) {
        throw ConfigException("No valid config value for hosts defined.");
    }

    for (Value::MemberIterator itr = hosts.MemberBegin(); itr != hosts.MemberEnd(); ++itr) {
        std::string host = itr->name.GetString();

        std::string root = document["hosts"][host.c_str()]["root"].GetString();
        std::string errorPagesDir = document["hosts"][host.c_str()]["errorpages"].GetString();
        std::string handler = document["hosts"][host.c_str()]["handler"].GetString();
        std::string additionalHeaders = "";
        if (document["hosts"][host.c_str()].HasMember("additionalHeaders")) {
            additionalHeaders = document["hosts"][host.c_str()]["additionalHeaders"].GetString();
        };

        this->hosts[host]["root"] = root;
        this->hosts[host]["errorPagesDir"] = errorPagesDir;
        this->hosts[host]["handler"] = handler;
        this->hosts[host]["additionalHeaders"] = additionalHeaders;

        int newPort;
        std::string port = host;
        if (port.find(":") == std::string::npos) {
            throw ConfigException("No port defined for host " + host);
        }
        else {
            port.erase(0, port.find(":") + 1);
            newPort = atoi(port.c_str());
        }
        this->ports.insert(newPort);
    }

}

Config* Config::_instance = 0;

Config::Config() {
};


Config* Config::getConfig(std::string path) {
    if (_instance == 0) {
        _instance = new Config();
        _instance->update(path);
    }
    return _instance;
}
