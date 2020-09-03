#ifndef FILERESPONSE_H
#define FILERESPONSE_H
#include <dirent.h>
#include <ctime>
#include <cstring>
#include <regex>
#include <stdio.h>
#include <sys/stat.h>
#include "../config/config.h"
#include "../logger/logger.h"
#include "../response/response.h"
#include "../request/request.h"
#include "../common/common.h"
#include "../exceptions/exceptions.h"


class FileResponse: public Response {
    public:
        std::string get();
        std::string post();
        std::string head();
        std::string options();
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
        std::string getLastModifiedTime();
        std::string getLastModifiedHeader();
        bool contentMatch();
        std::string makeEtag();
        std::string getETag();
};


#endif
