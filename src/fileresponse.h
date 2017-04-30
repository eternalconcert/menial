#ifndef FILERESPONSE_H
#define FILERESPONSE_H
#include "response.h"
#include "request.h"


class FileResponse: public Response {
    public:
        std::string get();
        std::string getHeader(std::string content, std::string fileName);
        std::string getFileName(std::string target);
        FileResponse(Request *request): Response (request) {};

    private:
        std::string guessFileType(std::string fileName);
};


#endif
