#include <stdexcept>
#include "exceptions.h"

Exception::Exception(std::string message) {
    this->message = message;
};

FileNotFoundException::FileNotFoundException(std::string message) {
    printf("FileNotFoundException: %s\n", message.c_str());
};

ConfigException::ConfigException(std::string message) {
    printf("ConfigException: %s\n", message.c_str());
    exit(1);
};

HandlerNotFound::HandlerNotFound(std::string message) {
    printf("HandlerNotFound: %s\n", message.c_str());
};

SSLError::SSLError(std::string message) {
    printf("SSLError: %s\n", message.c_str());
};
