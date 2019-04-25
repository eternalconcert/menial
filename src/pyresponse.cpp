#include <stdio.h>
#include <Python.h>
#include "common.h"
#include "exceptions.h"
#include "logger.h"
#include "pyresponse.h"


std::map<std::string, PyFunc*> pyFunctionMap;


PyFunc::PyFunc(std::string root) {
    Py_Initialize();

    PyObject *pName;
    pName = PyString_FromString(root.c_str());

    PyObject *pModule;
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    this->pFunc = PyObject_GetAttrString(pModule, "application");

};


PyFunc* PyFunc::getPyFunc(std::string root) {
    if (pyFunctionMap.find(root) == pyFunctionMap.end()) {
        PyFunc* instance = new PyFunc(root);
        pyFunctionMap[root] = instance;
    }
    return pyFunctionMap[root];
}



std::string PyFunc::getValue(PyObject *pArgs) {
    PyObject *pValue;
    pValue = PyObject_CallObject(pFunc, pArgs);
    if (PyErr_Occurred()) {
        PyErr_Print();
        Py_DECREF(pValue);
        Py_DECREF(pArgs);
    }
    std::string response = PyString_AsString(pValue);
    return response;
};


std::string PyResponse::get() {

    PyObject *pArgs;
    pArgs = PyTuple_New(4);

    PyTuple_SetItem(pArgs, 0, PyString_FromString(this->getRequest()->getVirtualHost().c_str()));
    PyTuple_SetItem(pArgs, 1, PyString_FromString(this->getRequest()->getTarget().c_str()));
    PyTuple_SetItem(pArgs, 2, PyString_FromString(this->getRequest()->getHeaders().c_str()));
    PyTuple_SetItem(pArgs, 3, PyString_FromString(this->getRequest()->getBody().c_str()));

    PyFunc* func = PyFunc::getPyFunc(this->config["root"]);
    std::string response = func->getValue(pArgs);
    return response;
}
