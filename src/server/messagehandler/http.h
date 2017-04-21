#ifndef HTTP_H
#define HTTP_H

#include <string>
#include "messagehandler.h"


class Request {
    public:
        void setHeader();
        void setBody();
        void setMethod();
        void setHost();
        void setTarget();

        std::string getMethod();
        std::string getHost();
        std::string getTarget();


        Request(std::string fullMessage);

    private:
        std::string message;
        std::string header;
        std::string body;
        std::string method;
        std::string target;
        std::string host;
};


class Response {
    public:
        std::string getHeader();
        void setStatus(int status);
        int getStatus();
        std::string getText();

        Response(Request *request);

    private:
        int status;
        Request *request;
        std::string getStatusMessage();
};


class HttpHandler: public MessageHandler {
    public:
        std::string handleMessage(std::string message);
        HttpHandler();
};


#endif
