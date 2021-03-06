#include "config.h"

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

    // IP logging
    this->iplogging = false;
    if (document.HasMember("iplogging")) {
        this->iplogging = document["iplogging"].GetBool();
    }

    // Debug
    this->debug = false;
    if (document.HasMember("debug")) {
        this->debug = document["debug"].GetBool();
    }

    // Error pages
    std::string globalstaticDir = "resources/staticdir/";
    if (document.HasMember("staticdir")) {
        globalstaticDir = document["staticdir"].GetString();
    }
    this->staticDir = globalstaticDir;

    // Resources
    std::string resources = "resources/";
    if (document.HasMember("resources")) {
        resources = document["resources"].GetString();
    }
    this->resources = resources;

    // hosts
    if (!document.HasMember("hosts")) {
        throw ConfigException("No config for hosts defined.");
    }

    Value& hosts = document["hosts"];
    if (!hosts.IsObject()) {
        throw ConfigException("No valid config value for hosts defined.");
    }

    for (Value::MemberIterator itr = hosts.MemberBegin(); itr != hosts.MemberEnd(); ++itr) {
        std::string host = itr->name.GetString();

        std::string root = "";
        if (document["hosts"][host.c_str()].HasMember("root")) {
            root = document["hosts"][host.c_str()]["root"].GetString();
        }
        this->hosts[host]["root"] = root;

        std::string rootPrefix = "";
        if (host.find("/") != std::string::npos) {
            rootPrefix = host;
            rootPrefix = rootPrefix.substr(rootPrefix.find("/"), std::string::npos);
        }
        this->hosts[host]["rootPrefix"] = rootPrefix;

        std::string handler = "file";
        if (document["hosts"][host.c_str()].HasMember("handler")) {
            handler = document["hosts"][host.c_str()]["handler"].GetString();
        };
        this->hosts[host]["handler"] = handler;

        if ((handler != "redirect" && handler != "proxy") && root == "") {
            throw ConfigException("No root specified for host: " + host);
        }

        std::string upstream = "";
        if (document["hosts"][host.c_str()].HasMember("upstream")) {
            upstream = document["hosts"][host.c_str()]["upstream"].GetString();
        }
        this->hosts[host]["upstream"] = upstream;

        if (handler == "proxy" && upstream == "") {
            throw ConfigException("No upstream specified for host: " + host);
        }

        std::string defaultfile = "";
        if (document["hosts"][host.c_str()].HasMember("defaultfile")) {
            defaultfile = document["hosts"][host.c_str()]["defaultfile"].GetString();
        }
        this->hosts[host]["defaultfile"] = defaultfile;

        std::string fallback = "";
        if (document["hosts"][host.c_str()].HasMember("fallback")) {
            fallback = document["hosts"][host.c_str()]["fallback"].GetString();
        }
        this->hosts[host]["fallback"] = fallback;

        bool dirlisting = false;
        if (document["hosts"][host.c_str()].HasMember("dirlisting")) {
            dirlisting = document["hosts"][host.c_str()]["dirlisting"].GetBool();
        }
        this->hosts[host]["dirlisting"] = dirlisting ? "true" : "false";

        std::string additionalheaders = "";
        if (document["hosts"][host.c_str()].HasMember("additionalheaders")) {
            additionalheaders = document["hosts"][host.c_str()]["additionalheaders"].GetString();
        };
        this->hosts[host]["additionalheaders"] = additionalheaders;

        std::string staticDir = this->staticDir;
        if (document["hosts"][host.c_str()].HasMember("staticdir")) {
            staticDir = document["hosts"][host.c_str()]["staticdir"].GetString();
        }
        this->hosts[host]["staticdir"] = staticDir;

        std::string defaultDoc = "index.html";
        if (document["hosts"][host.c_str()].HasMember("defaultdocument")) {
            defaultDoc = document["hosts"][host.c_str()]["defaultdocument"].GetString();
        }
        this->hosts[host]["defaultdocument"] = defaultDoc;

        std::string authFile = "";
        if (document["hosts"][host.c_str()].HasMember("authfile")) {
            authFile = document["hosts"][host.c_str()]["authfile"].GetString();
        }
        this->hosts[host]["authfile"] = authFile;

        std::string target = "";
        if (document["hosts"][host.c_str()].HasMember("target")) {
            target = document["hosts"][host.c_str()]["target"].GetString();
        }
        this->hosts[host]["target"] = target;

        bool permanent = false;
        if (document["hosts"][host.c_str()].HasMember("permanent")) {
            permanent = document["hosts"][host.c_str()]["permanent"].GetBool();
        }
        this->hosts[host]["permanent"] = permanent ? "true" : "false";

        bool keeptarget = false;
        if (document["hosts"][host.c_str()].HasMember("keeptarget")) {
            keeptarget = document["hosts"][host.c_str()]["keeptarget"].GetBool();
        }
        this->hosts[host]["keeptarget"] = keeptarget ? "true" : "false";

        // Disable TRACE Verb
        bool disableTrace = false;
        if (document["hosts"][host.c_str()].HasMember("disabletrace")) {
            disableTrace = document["hosts"][host.c_str()]["disabletrace"].GetBool();
        }
        this->hosts[host]["disabletrace"] = disableTrace ? "true" : "false";

        int portNum;
        std::string port = host;
        if (port.find(":") == std::string::npos) {
            throw ConfigException("No port defined for host " + host);
        }
        else {
            port.erase(0, port.find(":") + 1);
            portNum = atoi(port.c_str());
            if (this->defaultHosts[port] == "") {
                this->defaultHosts[port] = host;
            }
        }
        this->ports.insert(portNum);
    }

    // ssl
    if (document.HasMember("ssl")) {
        Value& ssl = document["ssl"];
        if (!ssl.IsObject()) {
            throw ConfigException("No valid config value for hosts defined.");
        }

        for (Value::MemberIterator itr = ssl.MemberBegin(); itr != ssl.MemberEnd(); ++itr) {

            std::string port = itr->name.GetString();
            int portNum = atoi(port.c_str());

            if (!document["ssl"][port.c_str()].HasMember("cert")) {
                throw ConfigException("No certificate path defined for ssl port " + port);
            }
            std::string certPath = document["ssl"][port.c_str()]["cert"].GetString();

            if (!document["ssl"][port.c_str()].HasMember("key")) {
                throw ConfigException("No key path defined for ssl port " + port);
            }
            std::string keyPath = document["ssl"][port.c_str()]["key"].GetString();

            if (!document["ssl"][port.c_str()].HasMember("on")) {
                throw ConfigException("No value defined for activation state of for ssl port " + port);
            }
            bool on = document["ssl"][port.c_str()]["on"].GetBool();

            this->sslPortmap[portNum]["cert"] = certPath;
            this->sslPortmap[portNum]["key"] = keyPath;
            this->sslPortmap[portNum]["on"] = on ? "true" : "false";
        }
    }
}

void Config::selfCheck() {
    std::map<std::string, std::map<std::string, std::string>>::iterator hostItr = this->hosts.begin();
    for (hostItr = this->hosts.begin(); hostItr != this->hosts.end(); hostItr++) {

        if (hostItr->second["handler"] == "file") {
            try {
                readFile(this->hosts[hostItr->first]["staticdir"] + "404.html");
            } catch (const FileNotFoundException &) {
                printf("404.html cannot be found for host: %s\n", hostItr->first.c_str());
                exit(1);
            };
        };
    }
}

Config* Config::_instance = 0;

Config::Config() {};


Config* Config::getConfig(std::string path) {
    if (_instance == 0) {
        _instance = new Config();
        _instance->update(path);
        _instance->selfCheck();
    }
    return _instance;
}
