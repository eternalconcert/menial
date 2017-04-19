#include <netinet/in.h>
#include <unistd.h>
#include "../common/common.h"
#include "cache.h"
#include "server.h"

#include "../common/logger.h"


int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Error: No port provided\n");
        exit(1);
    }

    int portno = atoi(argv[1]);
    Cache* cache = new Cache();

    Logger logger = getLogger();
    logger.info("Initializing server");
    Server server = Server(portno);
    server.setMessageHandler(cache);
    logger.info("Starting server");
    server.run();

    delete cache;

    return 0;
}
