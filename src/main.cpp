#include "httphandler.h"
#include "logger.h"
#include "server.h"


int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Error: No port provided\n");
        exit(1);
    }

    int portno = atoi(argv[1]);
    HttpHandler* httpHandler = new HttpHandler();

    Logger mainLogger = getLogger();
    mainLogger.info("Initializing server");
    Server server = Server(portno);
    server.setRequestHandler(httpHandler);
    mainLogger.info("Starting server");
    server.run();

    delete httpHandler;

    return 0;
}
