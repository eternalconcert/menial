#include <netinet/in.h>
#include <unistd.h>
#include "server.h"
#include "../common/common.h"
#include "../common/logger.h"
#include "messagehandler/http.h"


int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Error: No port provided\n");
        exit(1);
    }

    int portno = atoi(argv[1]);
    HttpHandler* httpHandler = new HttpHandler();

    Logger logger = getLogger();
    logger.info("Initializing server");
    Server server = Server(portno);
    server.setMessageHandler(httpHandler);
    logger.info("Starting server");
    server.run();

    delete httpHandler;

    return 0;
}
