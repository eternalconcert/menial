#include "config.h"
#include "requesthandler.h"
#include "logger.h"
#include "server.h"

Config config = Config();
RequestHandler* requestHandler = new RequestHandler();
Logger mainLogger = getLogger();


int main(int argc, char *argv[]) {

    mainLogger.info("Starting menial");
    mainLogger.info("Initializing server");
    Server server = Server(config.port);
    server.setRequestHandler(requestHandler);
    mainLogger.info("Starting server");
    server.run();

    delete requestHandler;

    return 0;
}
