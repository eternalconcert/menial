#include "config.h"
#include "logger.h"
#include "server.h"


Logger* mainLogger = Logger::getLogger();
static Config* config = Config::getConfig();


int main(int argc, char *argv[]) {
    mainLogger->info("Starting menial");
    mainLogger->info("Initializing server");
    Server server = Server(config->port);
    mainLogger->info("Starting server");
    server.run();

    return 0;
}
