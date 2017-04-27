#ifndef REQUEST_H
#define REQUEST_H
#include "requesthandler.h"


class Request {
    public:
        void setHeader();
        void setBody();
        void setMethod();
        void setHost();
        void setTarget();
        void setUserAgent();

        std::string getMethod();
        std::string getHost();
        std::string getTarget();
        std::string getGetParamsString(std::string target);
        std::string getUserAgent();

        Request(std::string fullMessage);

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
