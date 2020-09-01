#ifndef SERVER_H
#define SERVER_H
#include <openssl/ssl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../config/config.h"
#include "../exceptions/exceptions.h"
#include "../logger/logger.h"
#include "../request/request.h"
#include "../common/common.h"


class Server {
    public:
        void run();
        void runPlain();
        std::string readPlain(int sockfd);
        std::string readSSL(SSL *sockfd);
        void runSSL();
        void sendReply(std::string replyMessage, int newsockfd);
        void sendError(int status, int sockfd);
        void checkHeaderLengh(int idx);
        Server(int portno, Config* config, Logger* logger);

    private:
        int portno;
        Logger *logger;
        Config *config;
};

#endif
