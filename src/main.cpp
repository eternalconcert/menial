#include "config.h"
#include "logger.h"
#include "server.h"

#include <list>
#include <set>
#include <thread>



void spawnThread(Server server) {
    server.run();
};


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("%s\n", "Please provide a config file.");
        exit(1);
    };
    std::string configPath = argv[1];
    static Config* config = Config::getConfig(configPath);
    Logger* logger = Logger::getLogger(config);

    logger->info("Starting menial");
    logger->info("Initializing servers");
    logger->info("Starting servers");

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

    return 0;
}
