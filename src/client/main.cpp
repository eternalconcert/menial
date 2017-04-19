#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../common/common.h"


std::string getIncomingMessage(int sockfd) {
    char buffer[BUFFER_SIZE];

    if (sockfd < 0) {
        error("Error: Cannot accept");
    }

    std::string result;
    char curChar;
    while (curChar != END_OF_MESSAGE) {
        bzero(buffer, BUFFER_SIZE);
        int messageSize;
        messageSize = read(sockfd, buffer, BUFFER_LIMIT);
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


int main(int argc, char *argv[]) {

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3) {
        fprintf(stderr, "Usage %s hostname port message\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    int option = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));


    if (sockfd < 0) {
        error("Error: Cannot open socket");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "No such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error: Cannot connect");
    }

    std::string message = argv[3];
    message += END_OF_MESSAGE;
    n = write(sockfd, message.c_str(), message.size());

    if (n < 0) {
        error("Error: Cannot write to socket");
    }

    std::string response = getIncomingMessage(sockfd);
    printf("%s\n", response.c_str());
    close(sockfd);
    return 0;
}
