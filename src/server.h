#ifndef SERVER_H
#define SERVER_H
#include <openssl/ssl.h>
#include "config.h"
#include "logger.h"
#include "request.h"


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
