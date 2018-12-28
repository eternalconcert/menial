#ifndef SERVER_H
#define SERVER_H
#include "config.h"
#include "logger.h"
#include "request.h"


class Server {
    public:
        void run();
        void runPlain();
        void runSSL();
        void sendReply(std::string replyMessage, int newsockfd);
        void sendError(int status, int sockfd);
        Server(int portno, Config* config, Logger* logger);

    private:
        int portno;
        Logger *logger;
        Config *config;
};

#endif
