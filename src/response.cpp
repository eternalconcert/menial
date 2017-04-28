#include <stdio.h>
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
        case 500:
            statusMessage = "500 Internal Server Error";
            break;
    }
    return statusMessage;
}


std::string Response::getFileName(std::string target) {
    return target.substr(target.find_last_of("/") + 1, target.length());
}


std::string Response::guessFileType(std::string fileName) {
    std::string charset = " charset=utf-8;";
    std::string fileType = "text/html;" + charset;;

    std::string extension = fileName.substr(fileName.find_last_of(".") + 1, fileName.length());

    responseLogger.debug("Filename extension: " + extension);
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

std::string Response::getHeader(std::string content, std::string fileName) {
    std::string header = "HTTP/1.0 ";
    header += this->getStatusMessage();
    header += "\n";
    header += "Content-Length: " + std::to_string(content.length()) + "\n";
    header += "Content-Type: " + this->guessFileType(fileName) + "\n";
    header += "\r\n";
    return header;
}

Request* Response::getRequest() {
    return this->request;
}

std::string fromFile(Response *response) {
    std::string target = response->getRequest()->getTarget();
    if (target == "/") {
        target = "index.html";
    }
    std::string content;
    try {
        content += readFile(hostRootDir + target);
    } catch (FileNotFoundException) {
        content += readFile(errorpagesDir + "404.html");
        response->setStatus(404);
        responseLogger.error("404: Unknown target requested: " + target);
    }

    std::string fileName = response->getFileName(target);

    std::string result;
    result = response->getHeader(content, fileName) + content;
    return result;
}

std::string fromProcess(Response *response) {
    FILE *f;
    char path[BUFFER_SIZE];
    f = popen("python testdata/testhost/pyresponse.py", "r");
    std::string content;
    while (fgets(path, BUFFER_SIZE, f) != NULL) {
        content += path;
    }
    int status = pclose(f);
    if (WEXITSTATUS(status) != 0) {
        content += readFile(errorpagesDir + "500.html");
        response->setStatus(500);
        responseLogger.error("500: Error while reading from python");
    }

    std::string result;
    result = response->getHeader(content, "python.html") + content;
    return result;
}

std::string Response::get() {
    // std::string content = fromFile(this);
    std::string content = fromProcess(this);
    return content;
}
