#include <stdio.h>
#include "common.h"
#include "exceptions.h"
#include "logger.h"
#include "pyresponse.h"


std::string PyResponse::getHeader(std::string content) {
    std::string header = this->headerBase();
    header += "Content-Length: " + std::to_string(content.length()) + "\n";
    header += "Content-Type: text/html\n";
    header += this->config["additionalheaders"];
    header += "\r\n";
    return header;
}


std::string PyResponse::get() {
    std::string hostName = this->getRequest()->getVirtualHost();
    std::string interfaceCall = "python " + this->config["root"];
    interfaceCall += " -s='" + hostName + "'";
    interfaceCall += " -t='" + this->getRequest()->getTarget() + "'";
    interfaceCall += " -p='" + this->getRequest()->getHeader() + "'";
    interfaceCall += " -b='" + this->getRequest()->getBody() + "'";
    this->logger->debug("Calling Python with: " + interfaceCall);

    FILE *f;
    char path[BUFFER_SIZE];
    f = popen(interfaceCall.c_str(), "r");
    std::string content;
    while (fgets(path, BUFFER_SIZE, f) != NULL) {
        content += path;
    }
    int status = pclose(f);
    if (WEXITSTATUS(status) != 0) {
        content += readFile(this->config["staticdir"] + "500.html");
        this->setStatus(500);
        this->logger->error("500: Error while reading from python");
    }

    std::string result;
    result = this->getHeader(content) + content;
    return result;
}
