#include <stdio.h>
#include "common.h"
#include "exceptions.h"
#include "logger.h"
#include "pyresponse.h"

Logger pyResonseLogger = getLogger();



std::string PyResponse::getHeader(std::string content) {
    std::string header = "HTTP/1.0 ";
    header += this->getStatusMessage();
    header += "\n";
    header += "Content-Length: " + std::to_string(content.length()) + "\n";
    header += "Content-Type: text/html\n";
    header += "\r\n";
    return header;
}


std::string PyResponse::get() {
    Config config = Config();
    std::string interfaceCall = "python " + config.rootDir;
    FILE *f;
    char path[BUFFER_SIZE];
    f = popen(interfaceCall.c_str(), "r");
    std::string content;
    while (fgets(path, BUFFER_SIZE, f) != NULL) {
        content += path;
    }
    int status = pclose(f);
    if (WEXITSTATUS(status) != 0) {
        content += readFile(config.errorPagesRootDir + "500.html");
        this->setStatus(500);
        pyResonseLogger.error("500: Error while reading from python");
    }

    std::string result;
    result = this->getHeader(content) + content;
    return result;

}
