#ifndef PYRESPONSE_H
#define PYRESPONSE_H
#include <Python.h>
#include <stdio.h>
#include "../config/config.h"
#include "../logger/logger.h"
#include "../response/response.h"
#include "../request/request.h"
#include "../common/common.h"
#include "../exceptions/exceptions.h"
#include "../logger/logger.h"


class WSGIAdapter {
    public:
        static WSGIAdapter* getWSGIAdapter(std::string root, Config *config, Logger *logger);
        PyObject* wsgiFunction;
        PyObject* wsgiApplication;
        std::string getValue(PyObject *pArgs);
        ~WSGIAdapter() {
            Py_Finalize();
        };

    private:
        Logger *logger;
        Config *config;
        WSGIAdapter(std::string root, Config *config, Logger *logger);
};


class PyResponse: public Response {
    public:
        std::string get();
        std::string post();
        std::string options();
        PyResponse(Request *request, Config *config, Logger *logger): Response (request, config, logger) {};
};

#endif
