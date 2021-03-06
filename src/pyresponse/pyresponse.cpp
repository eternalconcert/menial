#include "pyresponse.h"


std::map<std::string, WSGIAdapter*> wsgiAdapterMap;


WSGIAdapter::WSGIAdapter(std::string root, Config *config, Logger *logger) {
    this->logger = logger;
    this->config = config;
    std::string rootCopy = root;  // No idea, but without something went wrong
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

    this->wsgiFunction = PyObject_GetAttrString(pModule, "wsgi_interface");
    const char* funcName = "application";
    const char* moduleName = rootCopy.c_str();
    if (rootCopy.find(":") != std::string::npos) {
        funcName = rootCopy.substr(rootCopy.find(":") + 1, std::string::npos).c_str();
        moduleName = rootCopy.substr(0, rootCopy.find(":")).c_str();
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
    if (PyErr_Occurred()) {
        PyErr_Print();
    }

    this->logger->debug("New instance ot WSGIAdapter created for root: " + root);
};


WSGIAdapter* WSGIAdapter::getWSGIAdapter(std::string root, Config *config, Logger *logger) {
    if (wsgiAdapterMap.find(root) == wsgiAdapterMap.end()) {
        WSGIAdapter* instance = new WSGIAdapter(root, config, logger);
        wsgiAdapterMap[root] = instance;
    }
    return wsgiAdapterMap[root];
}



std::string WSGIAdapter::getValue(PyObject *pArgs) {
    PyObject *pValue;
    if (PyErr_Occurred()) {
        PyErr_Print();
    }
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
    std::string body = PyBytes_AsString(pyBody);
    body = base64decode(body);
    this->logger->debug("PyResponseHeaders: " + headers);
    this->logger->debug("PyResponseBody: " + body);
    return headers + "\n" + body;
};


std::string PyResponse::get() {
    WSGIAdapter* adaptor = WSGIAdapter::getWSGIAdapter(this->hostConfig["root"], this->config, this->logger);

    PyObject *pArgs;
    pArgs = PyTuple_New(6);

    PyTuple_SetItem(pArgs, 0, adaptor->wsgiApplication);
    PyTuple_SetItem(pArgs, 1, PyUnicode_FromString(this->getRequest()->getVirtualHost().c_str()));
    PyTuple_SetItem(pArgs, 2, PyUnicode_FromString(this->getRequest()->getTarget().c_str()));
    PyTuple_SetItem(pArgs, 3, PyUnicode_FromString(this->getRequest()->getHeaders().c_str()));
    PyTuple_SetItem(pArgs, 4, PyBytes_FromString(this->getRequest()->getBody().c_str()));
    PyTuple_SetItem(pArgs, 5, PyBytes_FromString(this->hostConfig["additionalheaders"].c_str()));

    std::string response = adaptor->getValue(pArgs);

    return response;
}

std::string PyResponse::options() {
    return this->get();
}

std::string PyResponse::post() {
    return this->get();
}