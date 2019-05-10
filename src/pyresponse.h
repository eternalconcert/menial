#ifndef PYRESPONSE_H
#define PYRESPONSE_H
#include <Python.h>
#include "config.h"
#include "logger.h"
#include "response.h"
#include "request.h"


class WSGIAdapter {
    public:
        static WSGIAdapter* getWSGIAdapter(std::string root, Logger *logger);
        PyObject* wsgiFunction;
        PyObject* wsgiApplication;
        std::string getValue(PyObject *pArgs);
        ~WSGIAdapter() {
            Py_Finalize();
        };

    private:
        Logger *logger;
        WSGIAdapter(std::string root, Logger *logger);
};



class PyResponse: public Response {
    public:
        std::string get();
        PyResponse(Request *request, Config *config, Logger *logger): Response (request, config, logger) {};
};

#endif
