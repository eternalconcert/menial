#include "http.h"
#include "../../common/common.h"
#include "../../common/exceptions.h"
#include "../../common/logger.h"

// To be made better
std::string hostRootDir = "src/server/testdata/";
std::string errorpagesDir = "src/server/testdata/errorpages/";

Logger _logger = getLogger();


Request::Request(std::string message) {
    this->message = message;
    this->setHeader();
    this->setBody();
    this->setMethod();
    this->setHost();
    this->setTarget();
}

void Request::setHeader() {
    std::string message = this->message;
    message.erase(message.find("\n\r\n"), message.length());
    this->header = message;

}

void Request::setBody() {
    std::string message = this->message;
    this->body = message.substr(message.find("\n\r\n"), message.length());
}


void Request::setMethod() {
    std::string header = this->header;
    this->method = header.substr(0, header.find(" "));
}

void Request::setHost() {
    std::string header = this->header;
    std::string fieldName = "Host: ";
    header.erase(0, header.find(fieldName) + fieldName.length());
    std::string host = header.substr(0, header.find("\n") - 1);
    this->host = host;
}

void Request::setTarget() {
    std::string header = this->header;
    header.erase(0, header.find(" ") + 1);
    header.erase(header.find(" "), header.length());
    this->target = header;
}

std::string Request::getMethod() {
    return this->method;
};
std::string Request::getHost() {
    return this->host;
};
std::string Request::getTarget() {
    return this->target;
};


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

std::string Response::getHeader() {
    std::string header = "HTTP/1.0 ";
    header += this->getStatusMessage();
    header += " \nContent-Type: text/html; charset=utf-8\n\r\n";
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

    body = this->getHeader() + body;
    return body;
}


std::string HttpHandler::handleMessage(std::string message) {
    Request *request = new Request(message);
    _logger.debug("RequestMethod: " + request->getMethod());
    _logger.debug("RequestHost: " + request->getHost());
    _logger.debug("RequestTarget: " + request->getTarget());

    Response response = Response(request);
    return response.getText();
}

HttpHandler::HttpHandler() {};

