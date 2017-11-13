#ifndef RESPONSE_H
#define RESPONSE_H
#include <string>
#include "request.h"

class Response {
    public:
        virtual std::string get() {
            return "";
        };
        Request* getRequest();
        void setStatus(int status);
        int getStatus();
        std::string getStatusMessage();
        Response(Request *request);

    protected:
        Request *request;
        int status;
};


#endif
