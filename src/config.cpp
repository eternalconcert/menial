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

    Value& port = document["port"];
    if (!port.IsInt()) {
        throw ConfigException();
    }
    this->port = port.GetInt();

    Value& logLevel = document["loglevel"];
    if (!logLevel.IsString()) {
        throw ConfigException();
    }

    LogLevel level = logLevelByName(logLevel.GetString());
    this->logLevel = level;

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
