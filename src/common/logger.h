#ifndef LOGGER_H
#define LOGGER_H
#include <string>


enum LogLevel {DEBUG = 10, INFO = 20, WARNING = 30, ERROR = 40, CRITICAL = 50, NOTSET = 100};


class LogHandler {
    public:
        virtual void log(std::string level, std::string message) {};
};


class DefaultHandler: public LogHandler {
    public:
        void log(std::string level, std::string message);
        DefaultHandler() {};
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
const LogLevel LEVEL = DEBUG;

#endif
