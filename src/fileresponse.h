#ifndef FILERESPONSE_H
#define FILERESPONSE_H
#include "response.h"
#include "request.h"


class FileResponse: public Response {
    public:
        std::string get();

        std::string getHeader(std::string content, std::string fileName);
        void setStatus(int status);
        int getStatus();
        Request* getRequest();
        std::string getFileName(std::string target);

        FileResponse(Request *request);
        int status;

    private:
        Request *request;
        std::string getStatusMessage();
        std::string guessFileType(std::string fileName);
};


#endif
