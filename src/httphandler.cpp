#include "common.h"
#include "config.h"
#include "httphandler.h"
#include "logger.h"
#include "response.h"


Logger httpHandlerLogger = getLogger();


std::string HttpHandler::handleRequest(std::string message) {
    Request *request = new Request(message);
    httpHandlerLogger.debug("RequestMethod: " + request->getMethod());
    httpHandlerLogger.debug("RequestHost: " + request->getHost());
    httpHandlerLogger.debug("RequestTarget: " + request->getTarget());

    Response response = Response(request);
    return response.getText();
}

HttpHandler::HttpHandler() {};
