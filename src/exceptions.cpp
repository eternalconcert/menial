#include <stdexcept>
#include "exceptions.h"

Exception::Exception(std::string message) {
    this->message = message;
};

FileNotFoundException::FileNotFoundException(std::string message) {
    std::string msg = "FileNotFoundException: " + message + "\n\nmenial terminated.";
    printf("%s\n", msg.c_str());
};

ConfigException::ConfigException(std::string message) {
    std::string msg = "ConfigException: " + message + "\n\nmenial terminated.";
    printf("%s\n", msg.c_str());
    exit(1);
};

HandlerNotFound::HandlerNotFound(std::string message) {
    std::string msg = "HandlerNotFound: " + message + "\n\nmenial terminated.";
    printf("%s\n", msg.c_str());
};

SSLError::SSLError(std::string message) {
    std::string msg = "SSLError: " + message + "\n\nmenial terminated.";
    printf("%s\n", msg.c_str());
    exit(1);
};
