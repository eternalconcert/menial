#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../common.h"

int main(int argc, char *argv[]) {

    int sockfd, portno, m, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFFER_SIZE];

    if (argc < 3) {
        fprintf(stderr, "Usage %s hostname port\n", argv[0]);
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

    printf("Please enter a message: ");
    bzero(buffer, BUFFER_SIZE);
    fgets(buffer, BUFFER_LIMIT, stdin);
    n = write(sockfd, buffer, strlen(buffer));

    if (n < 0) {
        error("Error: Cannot write to socket");
    }

    bzero(buffer, BUFFER_SIZE);
    m = read(sockfd, buffer, BUFFER_LIMIT);
    if (m < 0) {
        error("Error: Cannot read from socket");
    }
    printf("%s\n", buffer);
    close(sockfd);
    return 0;
}
