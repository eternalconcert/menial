#include "redirectresponse.h"

std::string RedirectResponse::get() {
    this->setStatus(302);
    if (this->hostConfig["permanent"] == "true") {
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
    std::string header = "HTTP/1.1 ";
    header += this->getStatusMessage();
    header += "\n";
    header += "Location: " + this->hostConfig["target"];
    if (this->hostConfig["keeptarget"] == "true") {
        header += this->getRequest()->getTarget();
    }
    header += "\n";
    header += "Server: menial\n";
    return header;
}

std::string RedirectResponse::getHeader() {
    std::string header = this->headerBase();
    header += this->hostConfig["additionalheaders"];
    header += "\r\n";
    return header;
}
