#include "proxyresponse.h"

std::string ProxyResponse::readFromUpstream() {
    int sockfd, bytesRead, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    int option = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));


    if (sockfd < 0) {
        this->logger->error("Error: Cannot open socket");
    }

    std::string upstreamHostName = this->hostConfig["upstream"];
    upstreamHostName.erase(0, upstreamHostName.find(":") + 3);
    std::string upstreamPortStr = upstreamHostName;

    upstreamHostName.erase(upstreamHostName.find(":"), std::string::npos);
    int upstreamPort = atoi(upstreamPortStr.erase(0, upstreamPortStr.find(":") + 1).c_str());

    server = gethostbyname(upstreamHostName.c_str());
    if (server == NULL) {
        fprintf(stderr, "No such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(upstreamPort);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        this->logger->error("Error: Cannot connect");
    }

    // std::string outMessage = "GET / HTTP/1.1\nHost: " + upstreamHostName + ":" +
    //     std::to_string(upstreamPort) + "\n\n" + this->request->getBody();

    std::string outMessage = this->request->getHeaders() + "\n\n" + this->request->getBody();
    n = write(sockfd, outMessage.c_str(), strlen(outMessage.c_str()));

    if (n < 0) {
        throw SocketError("Error: Cannot write to socket");
    }

    char buffer[BUFFER_SIZE];
    std::string result;
    bzero(buffer, BUFFER_SIZE);
    do {
        bytesRead = recv(sockfd, buffer, BUFFER_LIMIT, 0);
        for (int j = 0; j < bytesRead; j++) {
            result += buffer[j];
        }
        if (bytesRead < 0) {
            throw SocketError("Error: Reading from socket");
        }
    } while (bytesRead > 0);

    close(sockfd);
    return result;
}


std::string ProxyResponse::get() {
    std::string content = this->getContent();
    return content;
}


std::string ProxyResponse::getContent() {
    std::string result;
    try {
        result = this->readFromUpstream();
    } catch (const SocketError &) {
        printf("%s\n", "ljdkfdl;kfglk;dfjl;kdsmglkdsfnmglkjdsfglk");
    }

    std::string preHeader = result;
    std::string postHeader = result;
    postHeader.erase(0, (postHeader.find("Server:") + postHeader.find("\n")));
    preHeader.erase(preHeader.find("Server:"), std::string::npos);

    return preHeader + "Server: menial\n" + postHeader;

}
