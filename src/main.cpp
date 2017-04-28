#include "config.h"
#include "logger.h"
#include "server.h"

Config config = Config();

Logger mainLogger = getLogger();


int main(int argc, char *argv[]) {

    mainLogger.info("Starting menial");
    mainLogger.info("Initializing server");
    Server server = Server(config.port);
    mainLogger.info("Starting server");
    server.run();

    return 0;
}
