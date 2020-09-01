#include "proxyresponse.h"

std::string ProxyResponse::readFromUpstream() {
    int sockfd, m, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFFER_SIZE];

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

    std::string outMessage = "GET / HTTP/1.0\nHost: " + upstreamHostName + ":" +
        std::to_string(upstreamPort) + "\n\n" + this->request->getBody();

    printf("%s", outMessage.c_str());
    n = write(sockfd, outMessage.c_str(), strlen(outMessage.c_str()));

    if (n < 0) {
        this->logger->error("Error: Cannot write to socket");
    }

    bzero(buffer, BUFFER_SIZE);
    m = read(sockfd, buffer, BUFFER_LIMIT);
    if (m < 0) {
        this->logger->error("Error: Cannot read from socket");
    }
    printf("Buffer: %s\n", buffer);
    close(sockfd);
    return "0";
}


std::string ProxyResponse::get() {
    std::string result;
    std::string content = this->getContent();
    result = content;
    return result;
}


std::string ProxyResponse::getContent() {
    std::string result = this->readFromUpstream();
    return result;
    //std::string content;
    //if (this->hostConfig["dirlisting"] == "true") {
    //    try {
    //        std::string targetPath = this->target;
    //        if (targetPath.rfind("/") == (targetPath.length() - 1)) {
    //            if (targetPath.find("..") != std::string::npos) {
    //                this->logger->warning("Intrusion try detected: " + targetPath);
    //                throw FileNotFoundException("Intrusion try detected: " + targetPath);
    //            }
    //            return this->getDirlisting();
    //        }
    //    } catch (const FileNotFoundException &) {
    //        this->setStatus(404);
    //        return readFile(this->hostConfig["staticdir"] + "404.html");
    //    }
    //}
//
    //try {
    //    content = readFile(filePath);
    //} catch (const FileNotFoundException &) {
    //    this->setStatus(404);
    //    return readFile(this->hostConfig["staticdir"] + "404.html");
    //}
    //return content;
}
