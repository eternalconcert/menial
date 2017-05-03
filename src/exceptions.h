#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H


class Exception: public std::exception {};

class FileNotFoundException: public Exception {
    using Exception::Exception;
};

class ConfigException: public Exception {
    using Exception::Exception;
};


#endif
