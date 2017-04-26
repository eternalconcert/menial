#include <map>
#include "logger.h"


void DefaultHandler::log(std::string level, std::string message) {
    std::map<std::string, std::string> logColors;

    logColors["debug"] = "\x1b[32m"; /* green */
    logColors["info"] = "\e[0m"; /* white */
    logColors["warning"] = "\x1b[33m"; /* yellow */
    logColors["error"] = "\x1b[31m"; /* red */
    logColors["critical"] = "\x1b[34m"; /* blue */
    logColors["reset"] = "\x1b[0m";

    printf("%s# [%s]: %s%s\n",
           logColors.find(level)->second.c_str(),
           level.c_str(),
           message.c_str(),
           logColors["reset"].c_str());
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


Logger getLogger() {
    Logger logger = Logger();
    logger.setLevel(config.logLevel);

    DefaultHandler* handler = new DefaultHandler();
    logger.setHandler(handler);
    return logger;
}
