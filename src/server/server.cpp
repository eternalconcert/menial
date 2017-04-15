#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "server.h"
#include "../common/common.h"

const int QUEUE_LENGTH = 5;


Server::Server(int portno) {
    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    this->portno = portno;

    int option = 1;
    setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    struct sockaddr_in serv_addr;

    if (this->sockfd < 0) {
        error("Error: Cannot open socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(this->portno);

    if (bind(this->sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error: Cannot bind");
    }
};

void Server::run() {
    listen(this->sockfd, QUEUE_LENGTH);
}

void Server::stop() {
    close(this->sockfd);
    close(this->newsockfd);
}

int Server::getSockFd() {
    return this->sockfd;
};

void Server::setNewSockFd() {
        socklen_t clilen;
        struct sockaddr_in cli_addr;
        clilen = sizeof(cli_addr);
        this->newsockfd = accept(this->sockfd, (struct sockaddr *) &cli_addr, &clilen);
}

int Server::getNewSockFd() {
        return this->newsockfd;
}

void Server::sendReply(std::string replyMessage) {

    int outMessage;
    outMessage = write(this->newsockfd, replyMessage.c_str(), sizeof(replyMessage));

    if (outMessage < 0) {
        error("Error: Writing to socket");
    }
}

std::string Server::getIncomingMessage() {
    char buffer[BUFFER_SIZE];

    if (this->newsockfd < 0) {
        error("Error: Cannot accept");
    }
    bzero(buffer, BUFFER_SIZE);
    int incoming_transmission;
    incoming_transmission = read(this->newsockfd, buffer, BUFFER_LIMIT);
    if (incoming_transmission < 0) {
        error("Error: Reading from socket");
    }

    return buffer;
}
