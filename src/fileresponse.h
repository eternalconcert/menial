#ifndef FILERESPONSE_H
#define FILERESPONSE_H
#include "config.h"
#include "logger.h"
#include "response.h"
#include "request.h"


class FileResponse: public Response {
    public:
        std::string get();
        std::string post();
        std::string head();
        std::string getHeader(std::string content, std::string fileName);

        std::string notFound();
        std::string notModified();

        std::string getDirlisting();
        void setFilePath();

        std::string filePath;
        std::string fileName;
        std::string target;

        FileResponse(Request *request, Config *config, Logger *logger): Response (request, config, logger) {
            this->target = getRequest()->getTarget();
            setFilePath();
            this->fileName = this->target.substr(this->target.find_last_of("/") + 1, this->target.length());
        };

    private:
        std::string guessFileType(std::string fileName);
        std::string getContent();
        std::string getLastModified();
        bool contentMatch();
        std::string makeEtag();
        std::string getETag();
};


#endif
