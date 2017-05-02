#include "config.h"
#include "logger.h"
#include "server.h"

#include <thread>

Logger* mainLogger = Logger::getLogger();
static Config* config = Config::getConfig();


void spanwServer(Server server) {
    server.run();
};


int main(int argc, char *argv[]) {
    mainLogger->info("Starting menial");
    mainLogger->info("Initializing server");
    Server server1 = Server(8000);
    Server server2 = Server(8080);

    mainLogger->info("Starting servers");
    std::thread thread1(spanwServer, server1);
    std::thread thread2(spanwServer, server2);

    thread1.join();
    thread2.join();

    return 0;
}
