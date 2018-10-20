#ifndef RESPONSE_H
#define RESPONSE_H
#include <string>
#include "config.h"
#include "logger.h"
#include "request.h"

class Response {
    public:
        virtual std::string get() {
            return "";
        };

        virtual std::string methodNotAllowed() {
            return "";
        };
        Request* getRequest();
        void setStatus(int status);
        int getStatus();
        std::string getStatusMessage();
        Response(Request *request, Config *config, Logger *logger);
        Config *config;
        Logger *logger;

    protected:
        Request *request;
        int status;
};

#endif
