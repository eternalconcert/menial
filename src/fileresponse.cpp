#include <stdio.h>
#include "common.h"
#include "exceptions.h"
#include "logger.h"
#include "fileresponse.h"
#include "config.h"


Logger* responseLogger = Logger::getLogger();


std::string FileResponse::get() {

    std::string target = this->getRequest()->getTarget();
    if (target == "/") {
        target = "index.html";
    }
    Config* config = Config::getConfig();
    std::string content;
    std::string hostName = this->getRequest()->getHost();

    if (hostName.find(":") == std::string::npos) {
        hostName += ":80";
        responseLogger->error(hostName);
    }
    try {
        content += readFile(config->hosts[hostName]["root"] + target);
    } catch (FileNotFoundException) {
        content += readFile(config->hosts[hostName]["errorPagesDir"] + "404.html");
        this->setStatus(404);
        responseLogger->error("404: Unknown target requested: " + target);
    }

    std::string fileName = this->getFileName(target);

    std::string result;
    result = this->getHeader(content, fileName) + content;
    return result;

    return content;
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

    responseLogger->debug("Filename extension: " + extension);
    if (extension == "css") {
        fileType = "text/css";
    }
    else if (extension == "png") {
        fileType = "image/png";
    }
    else if (extension == "mp3") {
        fileType = "audio/mpeg";
    }
    return fileType;

}
