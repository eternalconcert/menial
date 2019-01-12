#include <stdio.h>
#include "common.h"
#include "exceptions.h"
#include "logger.h"
#include "redirectresponse.h"

std::string RedirectResponse::get() {
    this->setStatus(302);
    if (this->config["permanent"] == "true") {
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
    std::string header = "HTTP/1.0 ";
    header += this->getStatusMessage();
    header += "\n";
    header += "Location: " + this->config["target"];
    if (this->config["keeptarget"] == "true") {
        header += this->getRequest()->getTarget();
    }
    header += "\n";
    header += "Server: menial\n";
    return header;
}

std::string RedirectResponse::getHeader() {
    std::string header = this->headerBase();
    header += this->config["additionalheaders"];
    header += "\r\n";
    return header;
}
