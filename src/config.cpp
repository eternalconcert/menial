#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "config.h"
#include "exceptions.h"

using namespace rapidjson;


void Config::update(std::string config) {
    Document document;
    ParseResult result = document.Parse(config.c_str());
    if (!result) {
        throw ConfigException();
    }

    // logLevel
    Value& logLevel = document["loglevel"];
    if (!logLevel.IsString()) {
        throw ConfigException();
    }
    LogLevel level = logLevelByName(logLevel.GetString());
    this->logLevel = level;

    // logFilePath
    Value& logFilePath = document["logfilepath"];
    if (!logFilePath.IsString()) {
        throw ConfigException();
    }
    this->logFilePath = logFilePath.GetString();

    // logFilePath
    Value& logger = document["logger"];
    if (!logger.IsString()) {
        throw ConfigException();
    }
    this->logger = logger.GetString();

    // hosts
    Value& hosts = document["hosts"];
    if (!hosts.IsObject()) {
        throw ConfigException();
    }

    for (Value::MemberIterator itr = hosts.MemberBegin(); itr != hosts.MemberEnd(); ++itr) {
        std::string host = itr->name.GetString();

        std::string root = document["hosts"][host.c_str()]["root"].GetString();
        std::string errorPagesDir = document["hosts"][host.c_str()]["errorpagesdir"].GetString();
        int port = document["hosts"][host.c_str()]["port"].GetInt();

        this->hosts[host]["root"] = root;
        this->hosts[host]["errorPagesDir"] = errorPagesDir;
        this->hosts[host]["port"] = std::to_string(port);

    }

}

Config* Config::_instance = 0;

Config::Config() {
    std::string json = readFile("testdata/menial.json");
    this->update(json);
};


Config* Config::getConfig() {
    if (_instance == 0) {
        _instance = new Config();
    }
    return _instance;
}
