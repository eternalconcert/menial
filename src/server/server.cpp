#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "server.h"
#include "../common/common.h"

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


std::string getIncomingMessage(int newsockfd) {
    char buffer[BUFFER_SIZE];

    if (newsockfd < 0) {
        error("Error: Cannot accept");
    }

    std::string result;
    char curChar;
    while (curChar != END_OF_MESSAGE) {
        bzero(buffer, BUFFER_SIZE);
        int messageSize;
        messageSize = read(newsockfd, buffer, BUFFER_LIMIT);
        if (messageSize < 0) {
            error("Error: Reading from socket");
        }

        for (int i = 0; i < messageSize; i++) {
            curChar = buffer[i];
            result += curChar;
        }
    }

    result = result.substr(0, result.size() - sizeof(END_OF_MESSAGE));
    return result;
}


Server::Server(int portno) {
    this->portno = portno;
};


void Server::run() {

    while (true) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        setUp(sockfd, this->portno);

        // Wait for connections
        listen(sockfd, QUEUE_LENGTH);

        int newsockfd = setNewSockFd(sockfd);

        // Message
        std::string incomingMessage = getIncomingMessage(newsockfd);
        MessageHandler *messageHandler = this->messageHandler;
        std::string replyMessage = messageHandler->handleMessage(incomingMessage);
        this->sendReply(replyMessage, newsockfd);

        // Close sockets
        close(sockfd);
        close(newsockfd);
    }

}


void Server::setMessageHandler(MessageHandler *messageHandler) {
    this->messageHandler = messageHandler;
}


void Server::sendReply(std::string replyMessage, int newsockfd) {

    replyMessage += END_OF_MESSAGE;
    int outMessageLen = write(newsockfd, replyMessage.c_str(), replyMessage.length());

    if (outMessageLen < 0) {
        error("Error: Writing to socket");
    }
}
