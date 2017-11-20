#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "config.h"
#include "logger.h"
#include "server.h"
#include "request.h"
#include "fileresponse.h"
#include "pyresponse.h"

const int QUEUE_LENGTH = 5;

Logger* logger = Logger::getLogger();
static Config* config = Config::getConfig();

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


std::string Server::getIncomingRequest(int newsockfd) {
    char buffer[BUFFER_SIZE];

    if (newsockfd < 0) {
        error("Error: Cannot accept");
    }

    std::string result;
    char curChar  = ' ';
    while (curChar != '\0') {
        bzero(buffer, BUFFER_SIZE);
        int messageSize;
        messageSize = read(newsockfd, buffer, BUFFER_LIMIT);
        if (messageSize < 0) {
            error("Error: Reading from socket");
        }

        for (int i = 0; i < BUFFER_LIMIT; i++) {
            curChar = buffer[i];
            result += curChar;
        }
    }

    result = result.substr(0, result.size());
    logger->debug("Server::getIncomingRequest result len: " + std::to_string(result.size()));
    logger->debug("Server::getIncomingRequest result: " + result);
    return result;
}


Server::Server(int portno) {
    logger->debug("Using portno: " + std::to_string(portno));
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
        std::string incomingMessage = this->getIncomingRequest(newsockfd);

        std::string replyMessage = this->getReplyMessage(incomingMessage);
        this->sendReply(replyMessage, newsockfd);

        // Close sockets
        close(sockfd);
        close(newsockfd);
    }
}

Response* getResponder(Request *request) {
    std::string responderName = config->hosts[request->getVirtualHost()]["responder"];

    if (responderName == "file") {
        return new FileResponse(request);
    }
    else if (responderName == "python") {
        return new PyResponse(request);
    }
    else {
        return new Response(request);
    }
}

std::string Server::getReplyMessage(std::string incomingMessage) {
        Request *request = new Request(incomingMessage);
        logger->debug("RequestMethod: " + request->getMethod());
        logger->debug("RequestHost: " + request->getHost());
        logger->debug("RequestTarget: " + request->getTarget());
        Response* response = getResponder(request);
        return response->get();
}

void Server::sendReply(std::string replyMessage, int newsockfd) {

    int outMessageLen = write(newsockfd, replyMessage.c_str(), replyMessage.length());
    logger->debug("Server::sendReply outMessageLen: " + std::to_string(outMessageLen));
    logger->debug("Server::sendReply replyMessage: " + replyMessage);

    if (outMessageLen < 0) {
        error("Error: Writing to socket");
    }
}
