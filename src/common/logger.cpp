#include <iostream>
#include "logger.h"


void DefaultHandler::log(std::string level, std::string message) {
    printf("# [%s]: %s\n", level.c_str(), message.c_str());
};


void Logger::setLevel(LogLevel level) {
    this->level = level;
}


void Logger::setHandler(LogHandler *handler) {
    this->handler = handler;
}


void Logger::debug(std::string message) {
    if (this->level <= DEBUG) {
        this->handler->log("DEBUG", message);
    }
}


void Logger::info(std::string message) {
    if (this->level <= INFO) {
        this->handler->log("INFO", message);
    }
}


void Logger::warning(std::string message) {
    if (this->level <= WARNING) {
        this->handler->log("WARNING", message);
    }
}


void Logger::error(std::string message) {
    if (this->level <= ERROR) {
        this->handler->log("ERROR", message);
    }
}


void Logger::critical(std::string message) {
    if (this->level <= CRITICAL) {
        this->handler->log("CRITICAL", message);
    }
}


Logger getLogger() {
    Logger logger = Logger();
    logger.setLevel(LEVEL);

    DefaultHandler* handler = new DefaultHandler();
    logger.setHandler(handler);
    return logger;
}