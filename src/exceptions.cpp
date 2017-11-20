#include <stdexcept>
#include "exceptions.h"

ConfigException::ConfigException(std::string message) {
    std::string msg = "ConfigException: " + message + "\n\nmenial terminated.";
    printf("%s\n", msg.c_str());
    exit(1);
};
