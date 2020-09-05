#ifndef REQUEST_H
#define REQUEST_H
#include <netinet/in.h>
#include <sstream>
#include "../common/common.h"
#include "../exceptions/exceptions.h"
#include "../config/config.h"
#include "../logger/logger.h"


class Request {
    public:

        void setClientIp(std::string ip);
        void parseMessage(std::string message);
        void setMethod();
        void setHostAndPort();
        void setTarget();
        void setUserAgent();
        bool authenticate();

        std::string getMethod();
        std::string getHeaders();
        std::string getBody();
        std::string getGetParams();
        std::string getVirtualHost();
        std::string getTarget();
        std::string getUserAgent();
        std::string getResponse();
        Request(std::string message, std::string client_ip, bool ssl, Config* config, Logger* logger);

    private:
        std::string headers;
        std::string body;
        std::string paramString;
        std::string method;
        std::string target;
        std::string host;
        std::string port;
        std::string userAgent;
        std::string clientIp;
        bool ssl;
        Config *config;
        Logger *logger;
};

#endif