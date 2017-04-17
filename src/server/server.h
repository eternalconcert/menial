#ifndef SERVER_H
#define SERVER_H
#include <string>
#include "cache.h"

class Server {
    public:
        void run();
        void setMessageHandler(MessageHandler*);
        void sendReply(std::string replyMessage, int newsockfd);
        MessageHandler *messageHandler;

        Server(int portno);

    private:
        int portno;

};

#endif
