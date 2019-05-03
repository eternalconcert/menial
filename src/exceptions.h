#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H


class Exception: public std::exception {

    public:
        std::string message;
        Exception(std::string message = "");
};

class ConfigException: public Exception {
    using Exception::Exception;

    public:
        ConfigException(std::string message = "");
};

class SSLError: public Exception {
    using Exception::Exception;

    public:
        SSLError(std::string message = "");
};

class FileNotFoundException: public Exception {
    using Exception::Exception;

    public:
        FileNotFoundException(std::string message = "");
};

class RequestHeaderFieldTooLarge: public Exception {
    using Exception::Exception;
};

class CouldNotParseHeaders: public Exception {
    using Exception::Exception;
};

class SocketError: public Exception {
    using Exception::Exception;
};

#endif
