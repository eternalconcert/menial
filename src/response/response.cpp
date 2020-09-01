#include "response.h"


std::string makeHostname(std::string fullHostName) {
    // TODO: Member function
    std::string hostName = fullHostName;
    hostName.erase(hostName.find(":"), std::string::npos);
    return hostName;
};


Response::Response(Request *request, Config *config, Logger *logger) {
    this->request = request;
    this->config = config;
    this->hostConfig = config->hosts[this->getRequest()->getVirtualHost()];
    this->logger = logger;
    this->fullHostName = this->getRequest()->getVirtualHost();
    this->hostName = makeHostname(this->fullHostName);
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


std::string Response::headerBase() {
    std::string header = "HTTP/1.0 ";
    header += this->getStatusMessage();
    header += "\n";
    header += "Server: menial\n";
    return header;
}


std::string Response::unauthorized() {
    this->logger->info("401: unauthorized");
    this->setStatus(401);
    std::string header = this->headerBase();
    header += "WWW-Authenticate: Basic realm = /" + HEADERDELIM;
    std::string content = readFile(this->hostConfig["staticdir"] + "401.html");
    return header + content;
}


std::string Response::getStatusMessage() {
    std::string statusMessage;
    switch (this->status) {
        case 200:
            statusMessage = "200 OK";
            break;
        case 204:
            statusMessage = "204 No Content";
            break;
        case 301:
            statusMessage = "301 Moved Permanently";
            break;
        case 302:
            statusMessage = "302 Moved Temporary";
            break;
        case 304:
            statusMessage = "304 Not Modified";
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


std::string Response::methodNotAllowed() {
    this->setStatus(405);
    std::string header = this->headerBase();
    return header + HEADERDELIM;
}


std::string Response::internalServerError() {
    this->logger->error("Internal server error");
    this->setStatus(500);
    std::string content = this->headerBase() + HEADERDELIM + readFile(this->hostConfig["staticdir"] + "500.html");
    return content;
}


std::string Response::empty() {
    this->setStatus(204);
    std::string header = this->headerBase() + "Allow: OPTIONS, GET, HEAD" + HEADERDELIM;
    return header;
}