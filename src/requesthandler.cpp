#include "requesthandler.h"
#include "logger.h"
#include "fileresponse.h"


Logger requestHandlerLogger = getLogger();

std::string RequestHandler::handle(std::string message) {
    Request *request = new Request(message);
    requestHandlerLogger.debug("RequestMethod: " + request->getMethod());
    requestHandlerLogger.debug("RequestHost: " + request->getHost());
    requestHandlerLogger.debug("RequestTarget: " + request->getTarget());

    FileResponse fileResponse = FileResponse(request);
    return fileResponse.get();
}

RequestHandler::RequestHandler() {};
