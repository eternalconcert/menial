#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "exceptions.h"
#include "server.h"
#include "request.h"

const int QUEUE_LENGTH = 100;


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
    int bytesReceived;
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (master_socket == 0) {
        this->logger->error("Unable to setup master_socket");
        throw SocketError();
    }
    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option))) {
        this->logger->error("Unable to setup socket options");
        throw SocketError();
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(this->portno);
    if (bind(master_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        this->logger->error("Cannot bind");
        throw SocketError();
    }
    if (listen(master_socket, QUEUE_LENGTH) < 0) {
        this->logger->error("Failed to listen");
        throw SocketError();
    }
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

        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            this->logger->error("Select failed");
            throw SocketError();
        }
        if (FD_ISSET(master_socket, &readfds)) {
            new_socket = accept(master_socket, (struct sockaddr *) &serv_addr, (socklen_t*)&addrlen);
            if (new_socket < 0) {
                this->logger->error("Failed to accept new socket");
                throw SocketError();
            }
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
                try {
                    Request *request = new Request(sd, inet_ntoa(serv_addr.sin_addr), this->config, this->logger);
                    this->sendReply(request->getResponse(), sd);
                } catch (RequestHeaderFieldTooLarge& e) {
                    this->sendError(431, sd);
                    this->logger->error(e.message);
                } catch (CouldNotParseHeaders& e) {
                    this->sendError(500, sd);
                    this->logger->error(e.message);
                } catch (SocketError& e) {
                    this->sendError(500, sd);
                    this->logger->error(e.message);
                }
                // Keep alive
                do {
                    bytesReceived = recv(sd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
                    this->logger->debug("Client sending " + std::to_string(bytesReceived) + " bytes.");
                } while (bytesReceived > 0);

                shutdown(sd, SHUT_RD);
                close(sd);
                client_socket[i] = 0;
            }
        }
    }
}


void Server::sendReply(std::string replyMessage, int sockfd) {

    int outMessageLen = send(sockfd, replyMessage.c_str(), replyMessage.length(), MSG_NOSIGNAL);
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

    int errorMessageLen = send(sockfd, header.c_str(), header.length(), MSG_NOSIGNAL);

    this->logger->debug("Server::sendError errorMessageLen: " + std::to_string(errorMessageLen));
    this->logger->debug("Server::sendError replyMessage: " + header);

    if (errorMessageLen < 0) {
        error("Error: Writing to socket");
    }
}
