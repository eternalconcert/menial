#include <fstream>
#include "http.h"


std::string readFromFile(std::string path) {
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

std::string HttpHandler::handleMessage(std::string message) {

	const std::string header = "HTTP/1.0 200 OK\nContent-Type: text/html; charset=utf-8\n\r\n";

    std::string reply = header;
    reply += readFromFile("src/server/testdata/index.html");

    return reply;
}

HttpHandler::HttpHandler() {};
