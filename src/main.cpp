#include "config.h"
#include "logger.h"
#include "server.h"

#include <list>
#include <map>
#include <thread>

Logger* mainLogger = Logger::getLogger();
static Config* config = Config::getConfig();


void spawnThread(Server server) {
    server.run();
};


// void joinThread(std::thread thread) {
//     thread.join();
// };


int main(int argc, char *argv[]) {
    mainLogger->info("Starting menial");
    mainLogger->info("Initializing servers");

    mainLogger->info("Starting servers");
    std::list<std::thread> threads;
    std::map<std::string, std::map<std::string, std::string>>::iterator hostItr = config->hosts.begin();
    for (hostItr = config->hosts.begin(); hostItr != config->hosts.end(); hostItr++) {
        Server server = Server(stoi(hostItr->second["port"]));

        threads.push_back(std::thread(spawnThread, server));
    }

    std::list<std::thread>::iterator threadItr = threads.begin();
    for (threadItr = threads.begin(); threadItr != threads.end(); threadItr++) {
        threadItr->join();
    }

    return 0;
}
