#ifndef CONFIG_H
#define CONFIG_H
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "exceptions.h"
#include "logger.h"

Logger configLogger = getLogger();

using namespace rapidjson;

void readConfig(std::string config) {


    Document document;
    ParseResult result = document.Parse(config.c_str());
    if (!result) {
        configLogger.error("Config: Cannot parse configuration file");
        throw ConfigException();
    }

    Value& hosts = document["hosts"];
    if (!hosts.IsArray()) {
        configLogger.error("Config: hosts key must be an array");
        throw ConfigException();
    }

    for (Value::ConstValueIterator itr = hosts.Begin(); itr != hosts.End(); ++itr) {
        configLogger.info(itr->GetString());
    }

    configLogger.info("Successfully read config");
}



#endif
