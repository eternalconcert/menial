#include "http.h"
#include "../../common/common.h"
#include "../../common/logger.h"

Logger _logger = getLogger();


std::string getMethod(std::string header) {
    return header.substr(0, header.find(" "));
}

std::string getHost(std::string header) {
    std::string fieldName = "Host: ";
    std::string host = header.substr(header.find(fieldName) + fieldName.size(), (header.find("\n") - fieldName.size()) - 1);
    return host;
}

std::string getTarget(std::string header) {
    std::string first = header.substr(header.find(" ") + 1, header.find("\n") - header.find(" "));
    std::string target = first.substr(0, first.find(" "));
    return target;
}

Request::Request(std::string message) {
    this->header = message.substr(0, message.find("\n\r\n"));
    this->body = message.substr(message.find("\n\r\n"), message.size());
    this->method = getMethod(this->header);
    this->host = getHost(this->header);
    this->target = getTarget(this->header);
}


std::string HttpHandler::handleMessage(std::string message) {
    Request request = Request(message);
    _logger.debug("RequestMethod: " + request.method);
    _logger.debug("RequestHost: " + request.host);
    _logger.debug("RequestTarget: " + request.target);

	const std::string header = "HTTP/1.0 200 OK\nContent-Type: text/html; charset=utf-8\n\r\n";

    std::string reply = header;
    std::string rootDir = "src/server/testdata/";

    reply += readFile(rootDir + request.target);

    return reply;
}

HttpHandler::HttpHandler() {};

