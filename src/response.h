#ifndef RESPONSE_H
#define RESPONSE_H
#include <string>
#include "config.h"
#include "logger.h"
#include "request.h"

static const std::string HEADERDELIM = "\n\n";

class Response {
    public:
        virtual std::string get() {
            return "";
        };
        virtual std::string post() {
            return "";
        };
        virtual std::string head() {
            return "";
        };
        virtual std::string notFound() {
            return "";
        };

        std::string methodNotAllowed();
        std::string internalServerError();

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
