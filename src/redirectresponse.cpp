#include <stdio.h>
#include "common.h"
#include "exceptions.h"
#include "logger.h"
#include "redirectresponse.h"

std::string RedirectResponse::get() {
    std::string hostName = this->getRequest()->getVirtualHost();
    this->setStatus(302);
    if (this->config->hosts[hostName]["permanent"] == "true") {
        this->setStatus(301);
    }
    std::string result = this->getHeader();
    return result;
}

std::string RedirectResponse::methodNotAllowed() {
    this->setStatus(405);
    std::string header = this->headerBase();
    header += "\r\n";
    return header;
}

std::string RedirectResponse::headerBase() {
    std::string hostName = this->getRequest()->getVirtualHost();
    std::string header = "HTTP/1.0 ";
    header += this->getStatusMessage();
    header += "\n";
    header += "Location: " + this->config->hosts[hostName]["target"];
    header += "\n";
    header += "Server: menial\n";
    return header;
}

std::string RedirectResponse::getHeader() {
    std::string header = this->headerBase();
    std::string hostName = this->getRequest()->getVirtualHost();
    header += this->config->hosts[hostName]["additionalHeaders"];
    header += "\r\n";
    return header;
}
