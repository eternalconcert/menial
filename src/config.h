#ifndef CONFIG_H
#define CONFIG_H
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "exceptions.h"
#include "logger.h"

using namespace rapidjson;

Logger configLogger = getLogger();


class Config {
    public:
        void update(std::string config) {
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
                configLogger.debug(std::string("Read configuration for host: ") + itr->GetString());
            }

            configLogger.debug("Successfully read config");
        }

        Config(std::string json) {
            this->update(json);
        }

};


extern Config config;

// cpp
Config config(readFile("testdata/menial.json"));

#endif
