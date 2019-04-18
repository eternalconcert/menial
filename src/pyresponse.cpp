#include <stdio.h>
#include <Python.h>
#include "common.h"
#include "exceptions.h"
#include "logger.h"
#include "pyresponse.h"


std::string PyResponse::get() {

    PyObject *pName;
    PyObject *pModule;
    PyObject *pFunc;
    PyObject *pValue;
    PyObject *pArgs;

    Py_Initialize();

    pName = PyString_FromString(this->config["root"].c_str());
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        this->logger->info("Python module found");
        pFunc = PyObject_GetAttrString(pModule, "application");
        if (!(pFunc && PyCallable_Check(pFunc))) {
            this->logger->error("No callable python function");
            this->setStatus(500);
            return this->headerBase();
        }
        pArgs = PyTuple_New(4);

        PyTuple_SetItem(pArgs, 0, PyString_FromString(this->getRequest()->getVirtualHost().c_str()));
        PyTuple_SetItem(pArgs, 1, PyString_FromString(this->getRequest()->getTarget().c_str()));
        PyTuple_SetItem(pArgs, 2, PyString_FromString(this->getRequest()->getHeaders().c_str()));
        PyTuple_SetItem(pArgs, 3, PyString_FromString(this->getRequest()->getBody().c_str()));

        pValue = PyObject_CallObject(pFunc, pArgs);
        if (PyErr_Occurred()) {
            PyErr_Print();
            Py_DECREF(pValue);
            Py_DECREF(pArgs);
        }

    }

    std::string response = PyString_AsString(pValue);
    Py_Finalize();
    return response;
}
