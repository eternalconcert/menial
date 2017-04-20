#include "http.h"

std::string header = "HTTP/1.0 200 OK\nContent-Type: text/html; charset=utf-8\n\r";

std::string HttpHandler::handleMessage(std::string message) {

    std::string reply = header;
    reply += "Hallo Welt!";

    return reply;
}

HttpHandler::HttpHandler() {};
