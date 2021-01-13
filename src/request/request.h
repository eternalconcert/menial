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
        std::string getMethod();
        std::string getHeaders();
        std::string getBody();
        std::string getGetParams();
        std::string getVirtualHost();
        std::string getTarget();
        std::string getUserAgent();
        std::string getEncodings();
        std::string getResponse();
        std::string target;

        bool authenticate();
        Request(std::string message, std::string client_ip, bool ssl, Config* config, Logger* logger);

    private:
        std::string headers;
        std::string body;
        std::string paramString;
        std::string method;
        std::string host;
        std::string port;
        std::string userAgent;
        std::string encodings;
        std::string clientIp;
        bool ssl;
        void setTarget();
        Config *config;
        Logger *logger;

        void setClientIp(std::string ip);
        void parseMessage(std::string message);
        void setMethod();
        void setHostAndPort();
        void setUserAgent();
        void setEncodings();
};

#endif
