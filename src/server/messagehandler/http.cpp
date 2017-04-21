#include "http.h"
#include "../../common/common.h"
#include "../../common/exceptions.h"
#include "../../common/logger.h"

Logger _logger = getLogger();


std::string getMethod(std::string header) {
    return header.substr(0, header.find(" "));
}

std::string getHost(std::string header) {
    std::string fieldName = "Host: ";
    header.erase(0, header.find(fieldName) + fieldName.length());
    std::string host = header.substr(0, header.find("\n") - 1);
    return host;
}

std::string getTarget(std::string header) {
    header.erase(0, header.find(" ") + 1);
    header.erase(header.find(" "), header.length());
    return header;
}

Request::Request(std::string message) {
    this->header = message.substr(0, message.find("\n\r\n"));
    this->body = message.substr(message.find("\n\r\n"), message.size());
    this->method = getMethod(this->header);
    this->host = getHost(this->header);
    this->target = getTarget(this->header);
}


std::string getStatusMessage(int status) {
    std::string statusMessage;
    switch (status) {
        case 200:
            statusMessage = "200 OK";
        case 404:
            statusMessage = "404 Not Found";
    }
    return statusMessage;
}


std::string buildHeader(int status) {
    std::string header = "HTTP/1.0 ";
    header += getStatusMessage(status);
    header += " \nContent-Type: text/html; charset=utf-8\n\r\n";
    return header;
}


std::string HttpHandler::handleMessage(std::string message) {
    Request request = Request(message);
    _logger.debug("RequestMethod: " + request.method);
    _logger.debug("RequestHost: " + request.host);
    _logger.debug("RequestTarget: " + request.target);


    std::string body;
    std::string rootDir = "src/server/testdata/";

    std::string target = request.target;
    if (request.target == "/") {
        target = "index.html";
    }

    // Build Response
    int status = 200;

    try {
        body += readFile(rootDir + target);
    } catch (FileNotFoundException) {
        std::string errorpagesDir = "src/server/testdata/errorpages/";
        body += readFile(errorpagesDir + "404.html");
        status = 404;
    }

	const std::string header = buildHeader(status);

    body = header + body;
    return body;
}

HttpHandler::HttpHandler() {};

