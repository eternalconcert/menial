#include "proxyresponse.h"

std::string readResponse(int sockfd) {
    std::string response;
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    int bytesRead;
    while((bytesRead = read(sockfd, buffer, BUFFER_LIMIT)) > 0) {
        for (int j = 0; j < bytesRead; j++) {
            response += buffer[j];
        }
    }
    return response;
}

std::string ProxyResponse::readFromUpstream() {
    int sockfd, n;
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
    std::string upstreamHost = upstreamHostName;
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

    std::string origMethodLine = this->request->getHeaders();
    // Rewrite headers
    origMethodLine = origMethodLine.substr(0, origMethodLine.find("\n"));
    std::string outMessage = origMethodLine + "\nHost: " +  upstreamHost + "\n\n" + this->request->getBody();
    // End rewrite headers
    n = write(sockfd, outMessage.c_str(), strlen(outMessage.c_str()));

    if (n < 0) {
        throw SocketError("Error: Cannot write to socket");
    }
    std::string upstreamResponse = readResponse(sockfd);
    close(sockfd);
    return upstreamResponse;
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
        this->logger->error("Cannot read from upstream");
    }

    // std::string preHeader = result;
    // std::string postHeader = result;
    // postHeader.erase(0, (postHeader.find("Server:") + postHeader.find("\n")));
    // preHeader.erase(preHeader.find("Server:"), std::string::npos);

    return result;
    //return preHeader + "Server: menial\n" + postHeader;
}
