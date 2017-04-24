#include "config.h"
#include "httphandler.h"
#include "logger.h"
#include "server.h"


int main(int argc, char *argv[]) {

    HttpHandler* httpHandler = new HttpHandler();

    Logger mainLogger = getLogger();
    mainLogger.info("Starting menial");

    mainLogger.info("Initializing server");
    Server server = Server(config.port);
    server.setRequestHandler(httpHandler);
    mainLogger.info("Starting server");
    server.run();

    delete httpHandler;

    return 0;
}
