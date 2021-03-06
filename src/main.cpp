#include <signal.h>
#include <list>
#include <set>
#include <thread>
#include "config/config.h"
#include "logger/logger.h"
#include "server/server.h"


void signalHandler(int signal) {

    switch (signal) {
        case SIGINT:
            printf("\nReceived SIGINT, %s\n", "terminated.");
            exit(0);
        case SIGTERM:
            printf("\nReceived SIGTERM, %s\n", "terminated.");
            exit(0);
    }
};


void spawnThread(Server server) {
    server.run();
};

#if TEST==0
int main(int argc, char *argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Ignore SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    if (argc < 2) {
        printf("%s\n", "Please provide a config file.");
        exit(1);
    };
    std::string configPath = argv[1];
    static Config* config = Config::getConfig(configPath);
    static Logger* logger = Logger::getLogger(config);

    logger->info("Starting menial");
    logger->info("Initializing servers");
    logger->info("Starting servers");

    std::map<std::string, std::map<std::string, std::string>>::iterator hostItr = config->hosts.begin();
    for (hostItr = config->hosts.begin(); hostItr != config->hosts.end(); hostItr++) {
        logger->info("Listening on host " + hostItr->first);
    }

    std::list<std::thread> threads;

    std::set<int>::iterator portItr = config->ports.begin();
    for (portItr = config->ports.begin(); portItr != config->ports.end(); portItr++) {
        Server server = Server(*portItr, config, logger);
        threads.push_back(std::thread(spawnThread, server));
    }

    std::list<std::thread>::iterator threadItr = threads.begin();
    for (threadItr = threads.begin(); threadItr != threads.end(); threadItr++) {
        threadItr->join();
    }
    delete config;
    delete logger;
    return 0;
}
#endif
