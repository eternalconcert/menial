#include "config.h"
#include "logger.h"
#include "server.h"

#include <list>
#include <set>
#include <thread>

Logger* mainLogger = Logger::getLogger();
static Config* config = Config::getConfig();


void spawnThread(Server server) {
    server.run();
};


int main(int argc, char *argv[]) {
    mainLogger->info("Starting menial");
    mainLogger->info("Initializing servers");
    mainLogger->info("Starting servers");
    std::list<std::thread> threads;

    std::set<int>::iterator portItr = config->ports.begin();
    for (portItr = config->ports.begin(); portItr != config->ports.end(); portItr++) {
        Server server = Server(*portItr);
        threads.push_back(std::thread(spawnThread, server));
    }

    std::list<std::thread>::iterator threadItr = threads.begin();
    for (threadItr = threads.begin(); threadItr != threads.end(); threadItr++) {
        threadItr->join();
    }

    return 0;
}
