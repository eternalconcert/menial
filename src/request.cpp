#include "request.h"


Request::Request(std::string message, Config* config, Logger* logger) {
    this->message = message;
    this->setHeader();
    this->setBody();
    this->setMethod();
    this->setHost();
    this->setTarget();
    this->setUserAgent();
    this->config = config;
    this->logger = logger;
    this->logger->info("Incoming " + this->getMethod() + " request | "
                        "Host: " + this->getHost() + " | "
                        "Target: " + this->getTarget() + " | "
                        "User-Agent: " + this->getUserAgent());
}

void Request::setHeader() {
    std::string message = this->message;
    message.erase(message.find("\n\r\n"), message.length() - 3);
    this->header = message;
}

void Request::setBody() {
    std::string message = this->message;
    message.erase(0, message.find("\n\r\n") + 3);
    this->body = message;
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

    this->target = target;
}

std::string Request::getMethod() {
    return this->method;
};

std::string Request::getHeader() {
    return this->header;
};

std::string Request::getBody() {
    return this->body;
};

std::string Request::getHost() {
    return this->host;
};

std::string Request::getVirtualHost() {
    std::string virtualHost = this->host;
    if (virtualHost.find(":") == std::string::npos) {
        virtualHost += ":80";
    }

    if (config->hosts[virtualHost]["responder"] == "") {
        virtualHost.erase(0, virtualHost.find(":"));
        virtualHost = "*" + virtualHost;
    };
    return virtualHost;
};

std::string Request::getTarget() {
    return this->target;
};

std::string Request::getUserAgent() {
    return this->userAgent;
};
