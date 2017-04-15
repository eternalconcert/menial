#include <netinet/in.h>
#include <unistd.h>
#include "../common/common.h"
#include "cache.h"
#include "server.h"

std::string ACTION_GET = "GET";
std::string ACTION_SET = "SET";
std::string DELIMITER = ":";


std::string process_message(std::string message, Cache *cache) {

    std::string action = message.substr(0, 3);
    std::string payload = message.substr(4, message.size() - 3);

    std::string reply;


    if (action == ACTION_GET) {
        reply = cache->get(payload);

    } else if (action == ACTION_SET) {
        std::string key = payload.substr(0, payload.find(DELIMITER));
        std::string value = payload.substr(payload.find(DELIMITER) + 1);
        cache->set(key, value);
        reply = "OK";
    }

    return reply;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Error: No port provided\n");
        exit(1);
    }

    int portno = atoi(argv[1]);

    Cache* cache = new Cache();

    while (true) {
        Server server = Server(portno);
        server.run();

        server.setNewSockFd();
        std::string incomingMessage = server.getIncomingMessage();

        std::string replyMessage = process_message(incomingMessage, cache);

        server.sendReply(replyMessage);

        server.stop();
    }
    delete cache;
    return 0;
}
