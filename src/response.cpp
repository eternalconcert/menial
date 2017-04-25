#include "common.h"
#include "exceptions.h"
#include "logger.h"
#include "response.h"


// To be made better
std::string hostRootDir = "testdata/testhost/";
std::string errorpagesDir = "testdata/testhost/errorpages/";


Logger responseLogger = getLogger();


Response::Response(Request *request) {
    this->request = request;
    this->setStatus(200);
}

void Response::setStatus(int status) {
    this->status = status;
};

int Response::getStatus() {
    return this->status;
};

std::string Response::getStatusMessage() {
    std::string statusMessage;
    switch (this->status) {
        case 200:
            statusMessage = "200 OK";
            break;
        case 404:
            statusMessage = "404 Not Found";
            break;
    }
    return statusMessage;
}


std::string Response::getFileName(std::string target) {
    return target.substr(target.find_last_of("/") + 1, target.length());
}


std::string Response::guessFileType(std::string fileName) {
    std::string fileType;
    std::string charset = " charset=utf-8;";

    std::string extension = fileName.substr(fileName.find_last_of(".") + 1, fileName.length());

    responseLogger.debug("Filename extension: " + extension);
    if (extension == "html") {
        fileType = "text/html; " + charset;
    } else if (extension == "png") {
        fileType = "image/png";
    }
    return fileType;

}


std::string Response::getHeader(std::string content, std::string fileName) {
    std::string header = "HTTP/1.0 ";
    header += this->getStatusMessage();
    header += "\n";
    header += "Content-Length: " + std::to_string(content.length()) + "\n";
    header += "Content-Type: " + this->guessFileType(fileName) + "\n";
    header += "\n\r\n";
    return header;
}


std::string Response::getText() {
    std::string content;

    std::string target = this->request->getTarget();
    if (target == "/") {
        target = "index.html";
    }

    try {
        content += readFile(hostRootDir + target);
    } catch (FileNotFoundException) {
        content += readFile(errorpagesDir + "404.html");
        this->setStatus(404);
    }

    std::string fileName = getFileName(target);

    std::string body;
    body = this->getHeader(content, fileName) + content;
    return body;
}
