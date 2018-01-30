#include <stdio.h>
#include "common.h"
#include "exceptions.h"
#include "logger.h"
#include "fileresponse.h"


std::string getGetParamsString(Request *request, Logger *logger) {
    std::string target = request->getHeader();
    target.erase(0, target.find(" ") + 1);
    target.erase(target.find(" "), target.length());

    std::string paramString = target;
    paramString.erase(0, target.find("?"));
    logger->debug("Get Request params " + paramString);
    return paramString;
};


std::string FileResponse::get() {
    std::string target = this->getRequest()->getTarget();
    std::string getParams = getGetParamsString(this->getRequest(), this->logger);

    if (getParams.length() > 0) {
        target.erase(target.find(getParams));
    }

    if (target == "/") {
        target = "index.html";
    }
    std::string content;
    std::string hostName = this->getRequest()->getVirtualHost();

    try {
        content += readFile(this->config->hosts[hostName]["root"] + target);
    } catch (FileNotFoundException) {
        content += readFile(this->config->hosts[hostName]["errorPagesDir"] + "404.html");
        this->setStatus(404);
        this->logger->warning("404: Unknown target requested: " + target);
    }

    std::string fileName = this->getFileName(target);

    std::string result;
    result = this->getHeader(content, fileName) + content;
    return result;
}


std::string FileResponse::getHeader(std::string content, std::string fileName) {
    std::string header = "HTTP/1.0 ";
    header += this->getStatusMessage();
    header += "\n";
    header += "Content-Length: " + std::to_string(content.length()) + "\n";
    header += "Content-Type: " + this->guessFileType(fileName) + "\n";
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
    else if (extension == "png") {
        fileType = "image/png";
    }
    else if (extension == "jpg") {
        fileType = "image/jpg";
    }
    else if (extension == "mp3") {
        fileType = "audio/mpeg";
    }
    return fileType;

}
