#include <arpa/inet.h>
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
    // SELECT
    fd_set readfds;
    int addrlen;
    int client_socket[30];
    int max_clients = 30;
    int max_sd;
    int new_socket;
    int option = 1;
    int sd;
    int valread;
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(this->portno);
    if (bind(master_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error: Cannot bind");
    }
    listen(master_socket, QUEUE_LENGTH);
    addrlen = sizeof(serv_addr);

    while(true) {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (FD_ISSET(master_socket, &readfds)) {
            new_socket = accept(master_socket, (struct sockaddr *) &serv_addr, (socklen_t*)&addrlen);
            for (int i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    break;
                }
            }
        }
        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds)) {
                // if ((valread = read(sd, buffer, BUFFER_SIZE)) <= 0) {
                //     getpeername(sd, (struct sockaddr*)&serv_addr, (socklen_t*)&addrlen);
                //     printf("Host disconnected, IP %s, port %d\n",
                //            inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
                // }
                // else {
                    try {
                        Request *request = new Request(sd, this->config, this->logger);
                        this->sendReply(request->getResponse(), sd);
                    } catch (RequestHeaderFieldTooLarge) {
                        this->sendError(431, sd);
                        this->logger->error("Client sent too many headers. Request caused a 431.");
                    } catch (CouldNotParseHeaders) {
                        this->sendError(500, sd);
                        this->logger->error("Could not parse headers.");
                    }
                    close(sd);
                    client_socket[i] = 0;
                }
            // }
        }
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
