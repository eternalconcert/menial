#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "exceptions.h"
#include "server.h"
#include "request.h"

const int QUEUE_LENGTH = 100;


int setUpSockFd(int portno) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
    // Wait for connections
    listen(sockfd, QUEUE_LENGTH);

    return sockfd;
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
        int sockfd = setUpSockFd(this->portno);
        int newsockfd = setNewSockFd(sockfd);

        try {
            Request *request = new Request(newsockfd, this->config, this->logger);
            this->sendReply(request->getResponse(), newsockfd);
        } catch (RequestHeaderFieldTooLarge) {
            this->sendError(431, newsockfd);
            this->logger->error("Client sent too many headers. Request caused a 431.");
        } catch (CouldNotParseHeaders) {
            this->sendError(500, newsockfd);
            this->logger->error("Could not parse headers.");
        }

        // Close sockets
        shutdown(sockfd, SHUT_WR);
        shutdown(newsockfd, SHUT_WR);

        // Keep alive
        char buffer[BUFFER_SIZE];
        int bytesReceived;
        do {
            bytesReceived = recv(sockfd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
            this->logger->debug("Client sending " + std::to_string(bytesReceived) + " bytes.");
        } while (bytesReceived > 0);

        shutdown(sockfd, SHUT_RD);
        shutdown(newsockfd, SHUT_RD);
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

std::string getErrorMessage(int status) {
    std::string errorMessage;
    switch (status) {
        case 431:
            errorMessage = "431 Request Header Fields Too Large";
            break;
    }
    return errorMessage;
}

void Server::sendError(int status, int sockfd) {

    std::string header = "HTTP/1.0 ";
    header += getErrorMessage(status);;
    header += "\n\r\n";

    int errorMessageLen = write(sockfd, header.c_str(), header.length());

    this->logger->debug("Server::sendError errorMessageLen: " + std::to_string(errorMessageLen));
    this->logger->debug("Server::sendError replyMessage: " + header);

    if (errorMessageLen < 0) {
        error("Error: Writing to socket");
    }
}
