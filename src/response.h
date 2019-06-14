#ifndef RESPONSE_H
#define RESPONSE_H
#include <string>
#include "config.h"
#include "logger.h"
#include "request.h"

class Response {
    public:
        virtual std::string get() {
            return "Virtual method get() called";
        };
        virtual std::string head() {
            return "Virtual method head() called";
        };
        virtual std::string methodNotAllowed() {
            return "Virtual method methodNotAllowed() called";
        };
        virtual std::string notFound() {
            return "Virtual method notFound() called";
        };
        virtual std::string internalServerError() {
            return "Virtual method internalServerError() called";
        };

        Request* getRequest();
        void setStatus(int status);
        int getStatus();
        std::string getStatusMessage();
        std::string headerBase();
        std::string unauthorized();
        std::string hostName;
        Response(Request *request, Config *config, Logger *logger);

    protected:
        Config *config;
        std::map<std::string, std::string> hostConfig;
        Logger *logger;
        Request *request;
        int status;
};

#endif
