#include <string>

class MessageHandler {
    public:
        virtual std::string handleMessage(std::string message) { return ""; };
        virtual ~MessageHandler() {};
};
