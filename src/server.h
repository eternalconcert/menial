#ifndef SERVER_H
#define SERVER_H


class Server {
    public:
        void run();
        void sendReply(std::string replyMessage, int newsockfd);

        Server(int portno);

    private:
        int portno;
        std::string getIncomingRequest(int newsockfd);
        std::string getReplyMessage(std::string incomingMessage);

};

#endif
