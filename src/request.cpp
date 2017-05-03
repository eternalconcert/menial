#include "logger.h"
#include "request.h"


Logger* requestLogger = Logger::getLogger();


Request::Request(std::string message) {
    this->message = message;
    this->setHeader();
    this->setBody();
    this->setMethod();
    this->setHost();
    this->setTarget();
    this->setUserAgent();
    requestLogger->info("Incoming " + this->getMethod() + " request | "
                        "Target: " + this->getTarget() + " | "
                        "User-Agent: " + this->getUserAgent());
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

void Request::setUserAgent() {
    std::string header = this->header;
    std::string fieldName = "User-Agent: ";
    header.erase(0, header.find(fieldName) + fieldName.length());
    std::string userAgent = header.substr(0, header.find("\n") - 1);
    this->userAgent = userAgent;
}

void Request::setTarget() {
    std::string target = this->header;
    target.erase(0, target.find(" ") + 1);
    target.erase(target.find(" "), target.length());

    std::string getParams = this->getGetParamsString(target);
    if (getParams.length() > 0) {
        target.erase(target.find(getParams));
    }

    this->target = target;
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

std::string Request::getGetParamsString(std::string target) {
    std::string paramString = target;
    paramString.erase(0, target.find("?"));
    requestLogger->debug("Get Request params " + paramString);
    return paramString;
};

std::string Request::getUserAgent() {
    return this->userAgent;
};
