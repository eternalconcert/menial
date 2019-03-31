#include <stdio.h>
#include "common.h"
#include "exceptions.h"
#include "logger.h"
#include "pyresponse.h"


std::string PyResponse::get() {
    std::string hostName = this->getRequest()->getVirtualHost();
    std::string interfaceCall = "python " + this->config["root"];
    interfaceCall += " -s='" + hostName + "'";
    interfaceCall += " -t='" + this->getRequest()->getTarget() + "'";
    interfaceCall += " -p='" + this->getRequest()->getHeaders() + "'";
    interfaceCall += " -b='" + this->getRequest()->getBody() + "'";
    this->logger->debug("Calling Python with: " + interfaceCall);

    FILE *f;
    char path[BUFFER_SIZE];
    f = popen(interfaceCall.c_str(), "r");
    std::string response;
    while (fgets(path, BUFFER_SIZE, f) != NULL) {
        response += path;
    }
    int status = pclose(f);
    if (WEXITSTATUS(status) != 0) {
        response += readFile(this->config["staticdir"] + "500.html");
        this->setStatus(500);
        this->logger->error("500: Error while reading from python");
    }

    return response;
}
