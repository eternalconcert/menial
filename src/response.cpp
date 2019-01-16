#include "response.h"

Response::Response(Request *request, Config *config, Logger *logger) {
    this->request = request;
    this->config = config->hosts[this->getRequest()->getVirtualHost()];
    this->logger = logger;
    this->hostName = this->getRequest()->getVirtualHost();
    this->setStatus(200);
}

Request* Response::getRequest() {
    return this->request;
};

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
        case 301:
            statusMessage = "301 Moved Permanently";
            break;
        case 302:
            statusMessage = "302 Moved Temporary";
            break;
        case 401:
            statusMessage = "401 Unauthorized";
            break;
        case 404:
            statusMessage = "404 Not Found";
            break;
        case 405:
            statusMessage = "405 Method Not Allowed";
            break;
        case 500:
            statusMessage = "500 Internal Server Error";
            break;
    }
    this->logger->info("Reply status: " + statusMessage);
    return statusMessage;
};
