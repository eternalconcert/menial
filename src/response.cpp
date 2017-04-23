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

std::string Response::getHeader(int contentLength) {
    std::string header = "HTTP/1.0 ";
    header += this->getStatusMessage();
    header += "\n";
    header += "Content-Lenght: " + std::to_string(contentLength) + "\n";
    header += "Content-Type: text/html; charset=utf-8\n";
    header += "\n\r\n";
    return header;
}

std::string Response::getText() {
    std::string body;

    std::string target = this->request->getTarget();
    if (target == "/") {
        target = "index.html";
    }

    try {
        body += readFile(hostRootDir + target);
    } catch (FileNotFoundException) {
        body += readFile(errorpagesDir + "404.html");
        this->setStatus(404);
    }

    body = this->getHeader(body.length()) + body;
    return body;
}
