#ifndef LOGGER_H
#define LOGGER_H
#include <cstring>
#include <ctime>
#include <fstream>
#include <map>
#include "../exceptions/exceptions.h"
#include "../config/config.h"

class LogHandler {
    public:
        virtual void log(std::string level, std::string message) = 0;
};


class DefaultLogHandler: public LogHandler {
    public:
        void log(std::string level, std::string message);
        DefaultLogHandler() {};
};

class FileLogHandler: public LogHandler {
    public:
        void log(std::string level, std::string message);
        FileLogHandler(Config* config);
        Config *config;
};

class Logger {
    public:
        void setLevel(LogLevel level);
        void setHandler(LogHandler *handler);

        void highlight(std::string message);
        void debug(std::string message);
        void info(std::string message);
        void warning(std::string message);
        void error(std::string message);
        void critical(std::string message);

        static Logger* getLogger(Config* config);

    private:
        Config *config;
        Logger() {};
        LogLevel level;
        LogHandler *handler;
        static Logger* _instance;
};


#endif
