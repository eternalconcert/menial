#include <iostream>
#include <map>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "../common.h"

std::string ACTION_GET = "GET";
std::string ACTION_SET = "SET";
std::string DELIMITER = ":";

class Cache {
    public:
        std::string get(std::string);
        void set(std::string key, std::string value);
        Cache();

    private:
        std::map<std::string, std::string> storage;
};

Cache::Cache() {};

std::string Cache::get(std::string key) {
    return this->storage[key];
}

void Cache::set(std::string key, std::string value) {
    this->storage[key] = value;
};

void init_server(int sockfd, int portno) {
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

    listen(sockfd, 5);
}

std::string process_message(std::string message, Cache *cache) {

    std::string action = message.substr(0, 3);
    std::string payload = message.substr(4, message.size() - 3);

    std::string reply;


    if (action == ACTION_GET) {
        reply = cache->get(payload);

    } else if (action == ACTION_SET) {
        std::string key = payload.substr(0, payload.find(DELIMITER));
        std::string value = payload.substr(payload.find(DELIMITER) + 1);
        cache->set(key, value);
        reply = "OK";
    }

    return reply;
}

void send_reply(int newsockfd, std::string reply_message) {
    int out_message;
    out_message = write(newsockfd, reply_message.c_str(), sizeof(reply_message));

    if (out_message < 0) {
        error("Error: Writing to socket");
    }
}

std::string process_transmission(int sockfd, int newsockfd) {
    char buffer[BUFFER_SIZE];

    if (newsockfd < 0) {
        error("Error: Cannot accept");
    }
    bzero(buffer, BUFFER_SIZE);
    int incoming_transmission;
    incoming_transmission = read(newsockfd, buffer, BUFFER_LIMIT);
    if (incoming_transmission < 0) {
        error("Error: Reading from socket");
    }

    return buffer;
}


int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Error: No port provided\n");
        exit(1);
    }

    int portno = atoi(argv[1]);
    int sockfd;

    Cache* cache = new Cache();

    while (true) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        init_server(sockfd, portno);

        socklen_t clilen;
        struct sockaddr_in cli_addr;
        clilen = sizeof(cli_addr);

        int newsockfd;
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        std::string incoming_message = process_transmission(sockfd, newsockfd);

        std::string reply_message = process_message(incoming_message, cache);

        send_reply(newsockfd, reply_message);

        close(newsockfd);
        close(sockfd);
    }
    delete cache;
    return 0;
}
