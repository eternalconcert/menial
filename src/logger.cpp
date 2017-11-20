#include <ctime>
#include <fstream>
#include <map>
#include "exceptions.h"
#include "logger.h"


void DefaultLogHandler::log(std::string level, std::string message) {
    std::map<std::string, std::string> logColors;

    logColors["debug"] = "\x1b[32m"; /* green */
    logColors["info"] = "\e[0m"; /* white */
    logColors["warning"] = "\x1b[33m"; /* yellow */
    logColors["error"] = "\x1b[31m"; /* red */
    logColors["critical"] = "\x1b[34m"; /* blue */
    logColors["reset"] = "\x1b[0m";

    time_t t = time(0);
    struct tm *now = localtime(&t);
    printf("%s[%s][%i-%i-%i %i:%i:%i]: %s%s\n",
           logColors.find(level)->second.c_str(),
           level.c_str(),
           (now->tm_year + 1900),
           (now->tm_mon + 1),
           now->tm_mday,
           now->tm_hour,
           now->tm_min,
           now->tm_sec,
           message.c_str(),
           logColors["reset"].c_str());
};


void FileLogHandler::log(std::string level, std::string message) {
    Config* config = Config::getConfig();
    std::ofstream logfile;
    logfile.open(config->logFilePath + "menial.log", std::ios_base::app);
    std::string logLine = "# [" + level + "]: " + message + "\n";
    logfile << logLine;
};


void Logger::setLevel(LogLevel level) {
    this->level = level;
}

void Logger::setHandler(LogHandler *handler) {
    this->handler = handler;
}

void Logger::debug(std::string message) {
    if (this->level <= DEBUG) {
        this->handler->log("debug", message);
    }
}

void Logger::info(std::string message) {
    if (this->level <= INFO) {
        this->handler->log("info", message);
    }
}

void Logger::warning(std::string message) {
    if (this->level <= WARNING) {
        this->handler->log("warning", message);
    }
}

void Logger::error(std::string message) {
    if (this->level <= ERROR) {
        this->handler->log("error", message);
    }
}

void Logger::critical(std::string message) {
    if (this->level <= CRITICAL) {
        this->handler->log("critical", message);
    }
}

Logger* Logger::_instance = 0;

Logger* Logger::getLogger() {
    if (_instance != 0) {
        return _instance;
    }
    Config* config = Config::getConfig();
    _instance = new Logger;
    _instance->setLevel(config->logLevel);

    LogHandler* handler;
    if (config->logger == "console") {
        handler = new DefaultLogHandler();
    }
    else if (config->logger == "file") {
        handler = new FileLogHandler();
    }
    else {
        throw ConfigException("No valid LogHandler defined.");
    }
    _instance->setHandler(handler);
    return _instance;
}
