#ifndef REQUEST_H
#define REQUEST_H
#include "config.h"
#include "logger.h"

class Request {
    public:
        void setHeaders(int sockfd);

        void setMethod();
        void setHostAndPort();
        void setTarget();
        void setUserAgent();
        void setBody();

        Config *config;
        Logger *logger;
        std::string getMethod();
        std::string getHeader();
        std::string getBody();
        std::string getVirtualHost();
        std::string getTarget();
        std::string getUserAgent();

        std::string getResponse();

        Request(int sockfd, Config* config, Logger* logger);

    private:
        std::string headers;
        std::string body;
        std::string method;
        std::string target;
        std::string host;
        std::string port;
        std::string userAgent;
};

#endif
