#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "exceptions.h"
#include "server.h"
#include "request.h"

const int QUEUE_LENGTH = 5;


void setUp(int sockfd, int portno) {
    int option = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    struct sockaddr_in serv_addr;

    if (sockfd < 0) {
        error("Error: Cannot open socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error: Cannot bind");
    }
}


int setNewSockFd(int sockfd) {
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    clilen = sizeof(cli_addr);
    return accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
}


Server::Server(int portno, Config* config, Logger* logger) {
    this->logger = logger;
    this->config = config;
    this->logger->debug("Using portno: " + std::to_string(portno));
    this->portno = portno;
};


void Server::run() {

    while (true) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        setUp(sockfd, this->portno);

        // Wait for connections
        listen(sockfd, QUEUE_LENGTH);

        int newsockfd = setNewSockFd(sockfd);
        try {
            Request *request = new Request(newsockfd, this->config, this->logger);
            this->sendReply(request->getResponse(), newsockfd);
        } catch (RequestHeaderFieldTooLarge) {
            this->logger->error("No handler for request errors implemented. Request caused a 431.");
        }


        // Close sockets
        close(sockfd);
        close(newsockfd);
    }
}

void Server::sendReply(std::string replyMessage, int sockfd) {

    int outMessageLen = write(sockfd, replyMessage.c_str(), replyMessage.length());
    this->logger->debug("Server::sendReply outMessageLen: " + std::to_string(outMessageLen));
    this->logger->debug("Server::sendReply replyMessage: " + replyMessage);

    if (outMessageLen < 0) {
        error("Error: Writing to socket");
    }
}
