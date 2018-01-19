#ifndef FILERESPONSE_H
#define FILERESPONSE_H
#include "config.h"
#include "logger.h"
#include "response.h"
#include "request.h"


class FileResponse: public Response {
    public:
        std::string get();
        std::string getHeader(std::string content, std::string fileName);
        std::string getFileName(std::string target);
        FileResponse(Request *request, Config *config, Logger *logger): Response (request, config, logger) {};

    private:
        std::string guessFileType(std::string fileName);
};


#endif
