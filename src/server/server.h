#ifndef SERVER_H
#define SERVER_H
#include <string>

class Server {
    public:
        void run();
        void stop();
        int getSockFd();
        void setNewSockFd();
        int getNewSockFd();
        void sendReply(std::string replyMessage);
        std::string getIncomingMessage();

        Server(int portno);
    private:
        int sockfd;
        int newsockfd;
        int portno;
};

#endif
