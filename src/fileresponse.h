#ifndef FILERESPONSE_H
#define FILERESPONSE_H
#include "config.h"
#include "logger.h"
#include "response.h"
#include "request.h"


class FileResponse: public Response {
    public:
        std::string get();
        std::string head();

        std::string headerBase();
        std::string methodNotAllowed();
        std::string getHeader(std::string content, std::string fileName);
        void setGetParamsString();
        void setFilePath();

        std::string paramString;
        std::string filePath;
        std::string fileName;
        std::string target;

        FileResponse(Request *request, Config *config, Logger *logger): Response (request, config, logger) {
            this->target = getRequest()->getTarget();
            setGetParamsString();
            setFilePath();
            this->fileName = this->target.substr(this->target.find_last_of("/") + 1, this->target.length());
        };

    private:
        std::string guessFileType(std::string fileName);
        std::string getContent();
        std::string getLastModified();
};


#endif
