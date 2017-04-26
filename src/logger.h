#ifndef LOGGER_H
#define LOGGER_H
#include "common.h"
#include "config.h"


class LogHandler {
    public:
        virtual void log(std::string level, std::string message) {};
};


class DefaultHandler: public LogHandler {
    public:
        void log(std::string level, std::string message);
        DefaultHandler() {};
};

class FileLogHandler: public LogHandler {
    public:
        void log(std::string level, std::string message);
        FileLogHandler() {};
};

class Logger {
    public:
        void setLevel(LogLevel level);
        void setHandler(LogHandler *handler);

        void debug(std::string message);
        void info(std::string message);
        void warning(std::string message);
        void error(std::string message);
        void critical(std::string message);

        Logger() {};

    private:
        LogLevel level;
        LogHandler *handler;
};

Logger getLogger();

#endif
