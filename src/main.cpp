#include "config.h"
#include "httphandler.h"
#include "logger.h"
#include "server.h"

Config config = Config();
HttpHandler* httpHandler = new HttpHandler();
Logger mainLogger = getLogger();


int main(int argc, char *argv[]) {

    mainLogger.info("Starting menial");
    mainLogger.info("Initializing server");
    Server server = Server(config.port);
    server.setRequestHandler(httpHandler);
    mainLogger.info("Starting server");
    server.run();

    delete httpHandler;

    return 0;
}
