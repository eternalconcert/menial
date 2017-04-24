#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "common.h"
#include "config.h"
#include "exceptions.h"
#include "logger.h"

using namespace rapidjson;


Logger configLogger = getLogger();


void Config::update(std::string config) {
    Document document;
    ParseResult result = document.Parse(config.c_str());
    if (!result) {
        configLogger.error("Config: Cannot parse config file");
        throw ConfigException();
    }

    Value& port = document["port"];
    if (!port.IsInt()) {
        configLogger.error("Config: port value must be an integer");
        throw ConfigException();
    }
    this->port = port.GetInt();
    configLogger.debug(std::string("Read config for port: ") + std::to_string(port.GetInt()));

    Value& hosts = document["hosts"];
    if (!hosts.IsArray()) {
        configLogger.error("Config: hosts value must be an array");
        throw ConfigException();
    }

    for (Value::ConstValueIterator itr = hosts.Begin(); itr != hosts.End(); ++itr) {
        // configLogger.debug(std::string("Read configuration for host: ") + itr->GetString());
    }

    configLogger.debug("Successfully read config");
}


Config::Config(std::string json) {
    this->update(json);
};


Config config(readFile("testdata/menial.json"));
