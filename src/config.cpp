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
    LogLevel level = logLevelByName("info");
    if (document.HasMember("loglevel")) {
        Value& logLevel = document["loglevel"];
        if (!logLevel.IsString()) {
            throw ConfigException("No valid config value for loglevel defined.");
        }
        level = logLevelByName(logLevel.GetString());
        if (level == NOTSET) {
            throw ConfigException("Unknown loglevel.");
        }
    }
    this->logLevel = level;

    // logFilePath
    std::string logFilePath = "/dev/null";
    if (document.HasMember("logfilepath")) {
        Value& logFilePathValue = document["logfilepath"];
        if (!logFilePathValue.IsString()) {
            throw ConfigException("No valid config value for logfilepath defined.");
        }
        logFilePath = logFilePathValue.GetString();
    }
    this->logFilePath = logFilePath;

    // logger
    std::string logger = "console";
    if (document.HasMember("logger")) {
        Value& loggerValue = document["logger"];
        if (!loggerValue.IsString()) {
            throw ConfigException("No valid config value for logger defined.");
        }
        logger = loggerValue.GetString();
    }
    this->logger = logger;

    // hosts
    Value& hosts = document["hosts"];
    if (!hosts.IsObject()) {
        throw ConfigException("No valid config value for hosts defined.");
    }

    for (Value::MemberIterator itr = hosts.MemberBegin(); itr != hosts.MemberEnd(); ++itr) {
        std::string host = itr->name.GetString();

        std::string root = document["hosts"][host.c_str()]["root"].GetString();
        this->hosts[host]["root"] = root;


        std::string handler = "file";
        if (document["hosts"][host.c_str()].HasMember("handler")) {
            handler = document["hosts"][host.c_str()]["handler"].GetString();
        };
        this->hosts[host]["handler"] = handler;

        std::string additionalHeaders = "";
        if (document["hosts"][host.c_str()].HasMember("additionalHeaders")) {
            additionalHeaders = document["hosts"][host.c_str()]["additionalHeaders"].GetString();
        };
        this->hosts[host]["additionalHeaders"] = additionalHeaders;

        std::string errorPagesDir = "default/errorpages/";
        if (document["hosts"][host.c_str()].HasMember("errorpages")) {
            errorPagesDir = document["hosts"][host.c_str()]["errorpages"].GetString();
        }
        this->hosts[host]["errorPagesDir"] = errorPagesDir;

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
