#ifndef RESPONSE_H
#define RESPONSE_H
#include "request.h"


class Response {
    public:
        std::string getHeader(std::string content, std::string fileName);
        void setStatus(int status);
        int getStatus();
        std::string get();
        Request* getRequest();
        std::string getFileName(std::string target);

        Response(Request *request);

    private:
        int status;
        Request *request;
        std::string getStatusMessage();
        std::string guessFileType(std::string fileName);
};


#endif
