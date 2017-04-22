#include "request.h"


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
