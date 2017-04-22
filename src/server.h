#ifndef SERVER_H
#define SERVER_H
#include "requesthandler.h"


class Server {
    public:
        void run();
        void setRequestHandler(RequestHandler*);
        void sendReply(std::string replyMessage, int newsockfd);
        RequestHandler *requestHandler;

        Server(int portno);

    private:
        int portno;

};

#endif
