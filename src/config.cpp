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

    // port
    Value& port = document["port"];
    if (!port.IsInt()) {
        throw ConfigException();
    }
    this->port = port.GetInt();

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

    // hosts
    Value& hosts = document["hosts"];
    if (!hosts.IsArray()) {
        throw ConfigException();
    }

    for (Value::ConstValueIterator itr = hosts.Begin(); itr != hosts.End(); ++itr) {
    }

}


Config::Config() {
    std::string json = readFile("testdata/menial.json");
    this->update(json);
};
