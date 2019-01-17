#include <netinet/in.h>
#include <sstream>
#include "common.h"
#include "exceptions.h"
#include "response.h"
#include "request.h"
#include "redirectresponse.h"
#include "fileresponse.h"
#include "pyresponse.h"


Response* _getHandler(Request *request, Config *config, Logger *logger) {
    /* Helper method to get the correct handler */
    std::string handlerName = config->hosts[request->getVirtualHost()]["handler"];
    if (handlerName == "file") {
        return new FileResponse(request, config, logger);
    }
    else if (handlerName == "python") {
        return new PyResponse(request, config, logger);
    }
    else if (handlerName == "redirect") {
        return new RedirectResponse(request, config, logger);
    }
    else {
        return new Response(request, config, logger);
    }
}


Request::Request(std::string headers, std::string client_ip, bool ssl, Config* config, Logger* logger) {
    this->config = config;
    this->logger = logger;
    this->ssl = ssl;

    this->setClientIp(client_ip);
    this->headers = headers;
    this->setMethod();
    this->setHostAndPort();
    this->setTarget();
    this->setUserAgent();
}


void Request::setClientIp(std::string ip) {
    this->clientIp = "xxx.xxx.xxx.xxx";
    if (this->config->iplogging) {
        this->clientIp = ip;
    }
    this->logger->info("Requesting client IP: " + this->clientIp);
}


void Request::setMethod() {
    std::string headers = this->headers;
    this->method = headers.substr(0, headers.find(" "));
}

void Request::setHostAndPort() {
    std::string headers = this->headers;
    std::string fieldName = "Host: ";
    headers.erase(0, headers.find(fieldName) + fieldName.length());
    std::string host = headers.substr(0, headers.find("\n") - 1);
    std::string port = ssl ? "443" : "80";

    if (host.find(":") != std::string::npos) {
        port = host;
        host = host.substr(0, host.find(":"));
        port = port.erase(0, host.length() + 1);
    }
    this->host = host;
    this->port = port;
    this->logger->info("Request on: " + host + ":" + port);
}

void Request::setTarget() {
    std::string target = this->headers;
    target.erase(0, target.find(" ") + 1);
    target.erase(target.find(" "), target.length());

    this->target = target;
}

void Request::setUserAgent() {
    std::string headers = this->headers;
    std::string fieldName = "User-Agent: ";
    headers.erase(0, headers.find(fieldName) + fieldName.length());
    std::string userAgent = headers.substr(0, headers.find("\n") - 1);
    this->userAgent = userAgent;
}

void Request::setBody() {
    this->body = "";
    // std::string message = this->message;
    // message.erase(0, message.find("\n\r\n") + 3);
    // this->body = message;
    this->logger->debug("RequestBody: " + this->body);
}


bool Request::authenticate() {
    std::string authFile = this->config->hosts[this->getVirtualHost()]["authfile"];
    if (authFile.empty()) {
        // Early exit
        return true;
    }

    this->logger->debug("Access is to " + this->getVirtualHost() + " is restricted by " + authFile);
    bool authenticated = false;
    std::string headers = this->headers;
    std::string authDelim = "Authorization: Basic ";
    if (headers.find(authDelim) != std::string::npos) {

        std::string credentials = headers.substr(headers.find(authDelim) + authDelim.length()); // End delimiter missing
        this->logger->debug("Requested raw credentials (base64): " + credentials);
        credentials = base64decode(credentials);
        std::string requestedPassword = credentials.substr(credentials.find(":") + 1);
        std::string requestedUsername = credentials.substr(0, credentials.find(":"));
        this->logger->debug("Requested username: " + requestedUsername);

        std::string db_content = readFile(authFile);
        std::string token;
        std::stringstream stream(db_content);
        while (std::getline(stream, token, '\n')) {
            if (token.find(requestedUsername + ":") != std::string::npos) {
                std::string passwordHash = token.substr(token.find(":") + 1, std::string::npos);
                authenticated = passwordHash == sha256hash(requestedPassword);
                if (authenticated) {
                    break;
                }
            }
        }
    }
    return authenticated;
}


std::string Request::getResponse() {
    Response* response = _getHandler(this, this->config, this->logger);
    bool authenticated;
    try {
        // Authenticate
        authenticated = this->authenticate();
    }
    catch (FileNotFoundException) {
        // Rewrite as response->sendError();
        response->setStatus(500);
        std::string header = "HTTP/1.0 ";
        header += response->getStatusMessage() + "\n";
        header += "Server: menial\n";
        header += "Content-Length: 0\n\n";
        std::string content =  readFile(this->config->hosts[this->getVirtualHost()]["errorPagesDir"] + "500.html");
        this->logger->error("Auth file cannot be read");
        return header + content;
    }
    if (!authenticated) {
        response->setStatus(401);
        std::string header = "HTTP/1.0 ";
        header += response->getStatusMessage() + "\n";
        header += "Server: menial\n";
        header += "WWW-Authenticate: Basic realm = /\n\n";
        return header;
    }

    if (this->getMethod() == "GET") {
        return response->get();
    }
    else if (this->getMethod() == "HEAD") {
        return response->head();
    }
    else {
        return response->methodNotAllowed();
    }
}

std::string Request::getMethod() {
    return this->method;
};

std::string Request::getHeader() {
    return this->headers;
};

std::string Request::getBody() {
    return this->body;
};

std::string Request::getVirtualHost() {
    std::string virtualHost = this->host + ":" + this->port;

    if (this->config->hosts[virtualHost]["handler"] == "") {
        virtualHost.erase(0, virtualHost.find(":"));
        virtualHost = "*" + virtualHost;
    };
    if (this->config->hosts[virtualHost]["handler"] == "") {
        virtualHost = this->config->defaultHosts[this->port];
    };
    return virtualHost;
};

std::string Request::getTarget() {
    return this->target;
};

std::string Request::getUserAgent() {
    return this->userAgent;
};
