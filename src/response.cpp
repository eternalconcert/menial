#include "response.h"

Response::Response(Request *request, Config *config, Logger *logger) {
    this->request = request;
    this->config = config;
    this->logger = logger;
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
        case 404:
            statusMessage = "404 Not Found";
            break;
        case 500:
            statusMessage = "500 Internal Server Error";
            break;
    }
    return statusMessage;
};
