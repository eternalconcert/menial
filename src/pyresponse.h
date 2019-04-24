#ifndef PYRESPONSE_H
#define PYRESPONSE_H
#include <Python.h>
#include "config.h"
#include "logger.h"
#include "response.h"
#include "request.h"


class PyFunc {
    public:
        static PyFunc* getPyFunc(std::string root);
        PyObject* pFunc;
        std::string getValue(PyObject *pArgs);
        ~PyFunc() {
            Py_Finalize();
        };

    private:
        PyFunc(std::string root);
};



class PyResponse: public Response {
    public:
        std::string get();
        PyResponse(Request *request, Config *config, Logger *logger): Response (request, config, logger) {};
};

#endif
