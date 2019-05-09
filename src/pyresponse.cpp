#include <stdio.h>
#include <Python.h>
#include "common.h"
#include "exceptions.h"
#include "logger.h"
#include "pyresponse.h"


std::map<std::string, WSGIAdapter*> wsgiAdapterMap;


WSGIAdapter::WSGIAdapter(std::string root) {
    Py_Initialize();
    PyObject *pName;
    pName = PyUnicode_FromString("wsgi_adapter");
    PyObject *pModule;
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (PyErr_Occurred()) {
        PyErr_Print();
        Py_DECREF(pName);
        Py_DECREF(pModule);
    }

    this->wsgiFunction = PyObject_GetAttrString(pModule, "wsgi");
    const char* funcName = "application";
    const char* moduleName = root.c_str();
    if (root.find(":") != std::string::npos) {
        funcName = root.substr(root.find(":") + 1, std::string::npos).c_str();
        moduleName = root.substr(0, root.find(":")).c_str();
    }

    PyObject *pyModule;
    pyModule = PyUnicode_FromString(moduleName);
    PyObject *module;
    module = PyImport_Import(pyModule);
    Py_DECREF(pyModule);

    if (PyErr_Occurred()) {
        PyErr_Print();
        Py_DECREF(module);
        Py_DECREF(pyModule);
    }

    this->wsgiApplication = PyObject_GetAttrString(module, funcName);
};


WSGIAdapter* WSGIAdapter::getWSGIAdapter(std::string root) {
    if (wsgiAdapterMap.find(root) == wsgiAdapterMap.end()) {
        WSGIAdapter* instance = new WSGIAdapter(root);
        wsgiAdapterMap[root] = instance;
    }
    return wsgiAdapterMap[root];
}



std::string WSGIAdapter::getValue(PyObject *pArgs) {
    PyObject *pValue;
    pValue = PyObject_CallObject(this->wsgiFunction, pArgs);
    if (PyErr_Occurred()) {
        PyErr_Print();
        Py_DECREF(pValue);
        Py_DECREF(pArgs);
    }

    PyObject *pyHeaders;
    pyHeaders = PyTuple_GetItem(pValue, 0);
    PyObject *pyBody;
    pyBody = PyTuple_GetItem(pValue, 1);
    std::string headers = PyBytes_AsString(pyHeaders);
    // std::string body = readFile("/home/christian/Pictures/1.png");
    std::string body = PyBytes_AsString(pyBody);
    body = base64decode(body);
    return headers + "\n" + body;
};


std::string PyResponse::get() {
    WSGIAdapter* adaptor = WSGIAdapter::getWSGIAdapter(this->hostConfig["root"]);

    PyObject *pArgs;
    pArgs = PyTuple_New(5);

    PyTuple_SetItem(pArgs, 0, adaptor->wsgiApplication);
    PyTuple_SetItem(pArgs, 1, PyUnicode_FromString(this->getRequest()->getVirtualHost().c_str()));
    PyTuple_SetItem(pArgs, 2, PyUnicode_FromString(this->getRequest()->getTarget().c_str()));
    PyTuple_SetItem(pArgs, 3, PyUnicode_FromString(this->getRequest()->getHeaders().c_str()));
    PyTuple_SetItem(pArgs, 4, PyUnicode_FromString(this->getRequest()->getBody().c_str()));

    std::string response = adaptor->getValue(pArgs);

    return response;
}
