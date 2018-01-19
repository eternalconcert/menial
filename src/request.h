#ifndef REQUEST_H
#define REQUEST_H
#include "config.h"
#include "logger.h"

class Request {
    public:
        void setHeader();
        void setBody();
        void setMethod();
        void setHost();
        void setTarget();
        void setUserAgent();

        std::string getMethod();
        std::string getHeader();
        std::string getBody();
        std::string getHost();
        std::string getVirtualHost();
        std::string getPort();
        std::string getTarget();
        std::string getUserAgent();
        Config *config;
        Logger* logger;

        Request(std::string message, Config* config, Logger* logger);

    private:
        std::string message;
        std::string header;
        std::string body;
        std::string method;
        std::string target;
        std::string host;
        std::string userAgent;
};

#endif
