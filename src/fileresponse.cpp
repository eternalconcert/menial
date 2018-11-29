#include <ctime>
#include <cstring>
#include <stdio.h>
#include <sys/stat.h>
#include "common.h"
#include "exceptions.h"
#include "logger.h"
#include "fileresponse.h"


void FileResponse::setGetParamsString() {

    std::string target = this->getRequest()->getHeader();
    target.erase(0, target.find(" ") + 1);
    target.erase(target.find(" "), target.length());

    std::string paramString = target;
    paramString.erase(0, target.find("?"));
    this->logger->debug("Get Request params " + paramString);
    this->paramString = paramString;
};


void FileResponse::setFilePath() {
    std::string target = this->getRequest()->getTarget();
    if (target == "/") {
        target = this->config["defaultdocument"];
        this->logger->debug("No file on / requested, using default target: " + target);
    }
    else if (target.back() == '/') {
            target = target + this->config["defaultdocument"];
            this->logger->debug("No file on subdir requested, using default target: " + target);
    }
    this->logger->debug("Requested document: " + target);

    if (this->paramString.length() > 0) {
        target.erase(target.find(this->paramString));
    }

    this->filePath = this->config["root"] + target;
    this->logger->debug("Filepath: " + this->filePath);
};


std::string FileResponse::headerBase() {
    std::string header = "HTTP/1.0 ";
    header += this->getStatusMessage();
    header += "\n";
    header += "Server: menial\n";
    return header;
}


std::string FileResponse::get() {
    std::string target = this->getRequest()->getTarget();

    if (this->paramString.length() > 0) {
        target.erase(target.find(this->paramString));
    }

    std::string content;
    try {
        content += readFile(filePath);
    } catch (FileNotFoundException) {
        content += readFile(this->config["errorPagesDir"] + "404.html");
        this->setStatus(404);
        this->logger->info("404: Unknown target requested: " + target);
    }

    std::string fileName = this->getFileName(target);

    std::string result;
    result = this->getHeader(content, fileName) + content;
    return result;
}


std::string FileResponse::methodNotAllowed() {
    this->setStatus(405);
    std::string header = this->headerBase();
    header += "\r\n";
    return header;
}


std::string FileResponse::getHeader(std::string content, std::string fileName) {
    std::string header = this->headerBase();
    header += "Content-Length: " + std::to_string(content.length()) + "\n";
    header += "Content-Type: " + this->guessFileType(fileName) + "\n";
    if (this->status == 200) {
        header += this->getLastModified(this->filePath);
    }
    header += this->config["additionalheaders"];
    header += "\r\n";
    return header;
}


std::string FileResponse::getFileName(std::string target) {
    return target.substr(target.find_last_of("/") + 1, target.length());
}


std::string FileResponse::guessFileType(std::string fileName) {
    std::string charset = " charset=utf-8;";
    std::string fileType = "text/html;" + charset;;

    std::string extension = fileName.substr(fileName.find_last_of(".") + 1, fileName.length());

    this->logger->debug("Filename extension: " + extension);
    if (extension == "css") {
        fileType = "text/css";
    }
    else if (extension == "log") {
        fileType = "text/plain";
    }
    else if (extension == "png") {
        fileType = "image/png";
    }
    else if (extension == "jpg") {
        fileType = "image/jpg";
    }
    else if (extension == "mp3") {
        fileType = "audio/mpeg";
    }
    else if (extension == "flac") {
        fileType = "audio/flac";
    }
    else if (extension == "tar") {
        fileType = "application/x-tar";
    }
    else if (extension == "gz") {
        fileType = "application/gzip";
    }
    return fileType;

}


std::string FileResponse::getLastModified(std::string filePath) {
    // Last-Modified: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
    char mtime[50];
    memset(mtime, 0, sizeof(mtime));
    struct stat fileInfo;
    stat(filePath.c_str(), &fileInfo);
    time_t t = fileInfo.st_mtime;
    struct tm lt;
    localtime_r(&t, &lt);
    strftime(mtime, sizeof(mtime), "Last-Modified: %a, %d %m %Y %H:%M:%S GMT\n", &lt);
    return std::string(mtime);
};
