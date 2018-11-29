#ifndef FILERESPONSE_H
#define FILERESPONSE_H
#include "config.h"
#include "logger.h"
#include "response.h"
#include "request.h"


class FileResponse: public Response {
    public:
        std::string headerBase();
        std::string get();
        std::string methodNotAllowed();
        std::string getHeader(std::string content, std::string fileName);
        std::string getFileName(std::string target);
        std::string getLastModified(std::string filePath);
        void setGetParamsString();
        void setFilePath();
        FileResponse(Request *request, Config *config, Logger *logger): Response (request, config, logger) {
            setGetParamsString();
            setFilePath();
        };

        std::string paramString;
        std::string filePath;


    private:
        std::string guessFileType(std::string fileName);
};


#endif
