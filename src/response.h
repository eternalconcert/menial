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

        virtual std::string head() {
            return "";
        };

        virtual std::string unauthorized() {
            return "";
        };

        virtual std::string methodNotAllowed() {
            return "";
        };

        virtual std::string notFound() {
            return "";
        };

        virtual std::string internalServerError() {
            return "";
        };

        Request* getRequest();
        void setStatus(int status);
        int getStatus();
        std::string getStatusMessage();
        std::string hostName;
        Response(Request *request, Config *config, Logger *logger);

    protected:
        std::map<std::string, std::string> config;
        Logger *logger;
        Request *request;
        int status;
};

#endif
