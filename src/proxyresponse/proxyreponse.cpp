#include "proxyresponse.h"

std::string readResponse(int sockfd, bool keepAlive) {
    std::string response;
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    int bytesRead;
    //if (keepAlive) {
    //    condition = (response.length() != expectedLength) || (expectedLength == 0);
    //}
    // size_t expectedLength = 0;
    // int contentLength = -1;
    while((bytesRead = recv(sockfd, buffer, BUFFER_LIMIT, SOCK_NONBLOCK)) > 0) {
        for (int j = 0; j < bytesRead; j++) {
            response += buffer[j];
        }
        // if (keepAlive) {
        //     if (!(contentLength > -1) && (response.find("Content-Length:") != std::string::npos)) {
        //         std::string contentLengthHeaderValue = response.substr(response.find("Content-Length:") + 16, response.find("\n", response.find("Content-Length:")));
        //         contentLength = std::stoi(contentLengthHeaderValue);
        //     }
        //     if ((expectedLength == 0) && (response.find("\n\n") != std::string::npos)) {
        //         expectedLength = response.find("\n\n") + 1 + contentLength;
        //     }
        // }
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
    std::string origHeadersRest = this->request->getHeaders();
    // TODO: Do not ignore keep-alive
    origHeadersRest = origHeadersRest.replace(origHeadersRest.find("Connection: keep-alive"), origHeadersRest.find("Connection: keep-alive") + 22, "Connection: close\n");
    origHeadersRest = origHeadersRest.substr(nthOccurance(origHeadersRest, "\n", 2) + 1, origHeadersRest.find("\n\n"));
    std::string outMessage = origMethodLine + "\nHost: " +  upstreamHost + "\n" + origHeadersRest + "\n\n" + this->request->getBody();
    // End rewrite headers
    bool keepAlive = origHeadersRest.find("Connection: keep-alive") != std::string::npos;
    n = write(sockfd, outMessage.c_str(), strlen(outMessage.c_str()));

    if (n < 0) {
        throw SocketError("Error: Cannot write to socket");
    }
    std::string upstreamResponse = readResponse(sockfd, keepAlive);
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
    } catch (const SocketError &error) {
        this->logger->error("Cannot read from upstream: " + error.message);
    }
    return result;
}
