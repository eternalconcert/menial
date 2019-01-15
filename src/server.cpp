#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "common.h"
#include "exceptions.h"
#include "server.h"
#include "request.h"

const int QUEUE_LENGTH = 100;


Server::Server(int portno, Config* config, Logger* logger) {
    this->logger = logger;
    this->config = config;
    this->portno = portno;
};


std::string Server::readPlainHeaders(int sockfd) {
    char buffer[BUFFER_SIZE];

    if (sockfd < 0) {
        throw SocketError("Error: Cannot accept");
    }

    std::string headers;
    bool foundEnd = false;
    int bytesReceived;

    int headerIdx = 0;
    do {
        bytesReceived = recv(sockfd, buffer, BUFFER_LIMIT, 0);
        if (bytesReceived < 0) {
            throw SocketError("Error: Reading from socket");
        }

        for (int j = 0; j < bytesReceived; j++) {
            headers += buffer[j];
            if(headers[headerIdx] == '\n' and headers[headerIdx-1] == '\r' and headers[headerIdx-2] == '\n') {
                foundEnd = true;
                break;
            }
            headerIdx++;
        }
        // Make sure it is not an TLS connection
        if (not isupper(headers[0])) {
            throw CouldNotParseHeaders("Not a plain connection. Maybe it is TLS?");
        }
    } while ((bytesReceived > 0 ) and not foundEnd and not (headerIdx > MAX_HEADER_LENGTH));

    // Check header length
    if (headerIdx > MAX_HEADER_LENGTH) {
        this->logger->warning(
            "Server::getIncomingRequest header length exceeded! Client sent too many headers."
        );
        throw RequestHeaderFieldTooLarge("Client sent too many headers. Request caused a 431.");
    }
    this->logger->debug("Server::getIncomingRequest header length: " + std::to_string(headers.size()));
    this->logger->debug("Server::getIncomingRequest header content:\n" + headers);

    if (headers.length() == 0) {
        throw CouldNotParseHeaders("Could not parse headers.");
    }
    return headers;
}


std::string Server::readSSLHeaders(SSL *sockfd) {
    char buffer[BUFFER_SIZE];

    if (sockfd < 0) {
        throw SocketError("Error: Cannot accept");
    }

    std::string headers;
    bool foundEnd = false;
    int bytesReceived;
    int headerIdx = 0;
    do {
        bytesReceived = SSL_read(sockfd, buffer, BUFFER_LIMIT);
        if (bytesReceived < 0) {
            throw SocketError("Error: Reading from socket");
        }

        for (int j = 0; j < bytesReceived; j++) {
            headers += buffer[j];
            if(headers[headerIdx] == '\n' and headers[headerIdx-1] == '\r' and headers[headerIdx-2] == '\n') {
                foundEnd = true;
                break;
            }
            headerIdx++;
        }
    } while ((bytesReceived > 0 ) and not foundEnd and not (headerIdx > MAX_HEADER_LENGTH));

    // Check header length
    if (headerIdx > MAX_HEADER_LENGTH) {
        this->logger->warning(
            "Server::getIncomingRequest header length exceeded! Client sent too many headers."
        );
        throw RequestHeaderFieldTooLarge("Client sent too many headers. Request caused a 431.");
    }
    this->logger->debug("Server::getIncomingRequest header length: " + std::to_string(headers.size()));
    this->logger->debug("Server::getIncomingRequest header content:\n" + headers);

    if (headers.length() == 0) {
        throw CouldNotParseHeaders("Could not parse headers.");
    }
    return headers;
}


int makeMasterSocket(Logger *logger) {
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (master_socket == 0) {
        logger->error("Unable to setup master_socket");
        throw SocketError();
    }
    int option = 1;
    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option))) {
        logger->error("Unable to setup socket options");
        throw SocketError();
    }
    return master_socket;
}


struct sockaddr_in makeServerAddr(int portno, int master_socket, Logger *logger) {
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(master_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        logger->error("Cannot bind");
        throw SocketError();
    }
    if (listen(master_socket, QUEUE_LENGTH) < 0) {
        logger->error("Failed to listen");
        throw SocketError();
    }
    return serv_addr;
}


void Server::runPlain() {
    this->logger->info("Setting up plain socket for port: " + std::to_string(portno));
    // SELECT
    fd_set readfds;
    int max_clients = 30;
    int client_socket[max_clients];
    int max_sd;
    int client;
    int sd;
    int bytesReceived;
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    int master_socket = makeMasterSocket(this->logger);
    struct sockaddr_in serv_addr = makeServerAddr(this->portno, master_socket, this->logger);
    int addrlen = sizeof(serv_addr);

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            this->logger->error("Select failed");
            throw SocketError();
        }
        if (FD_ISSET(master_socket, &readfds)) {
            client = accept(master_socket, (struct sockaddr*)&serv_addr, (socklen_t*)&addrlen);
            if (client < 0) {
                this->logger->error("Failed to accept new socket");
                throw SocketError();
            }
            for (int i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = client;
                    break;
                }
            }
        }

        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds)) {
                try {
                    std::string headers = this->readPlainHeaders(sd);
                    Request *request = new Request(headers, inet_ntoa(serv_addr.sin_addr), false, this->config, this->logger);
                    this->sendReply(request->getResponse(), sd);
                } catch (std::out_of_range) {
                    this->sendError(400, sd);
                    this->logger->error("Bad request");
                } catch (RequestHeaderFieldTooLarge& e) {
                    this->sendError(431, sd);
                    this->logger->warning(e.message);
                } catch (CouldNotParseHeaders& e) {
                    this->sendError(500, sd);
                    this->logger->error(e.message);
                } catch (SocketError& e) {
                    this->sendError(500, sd);
                    this->logger->error(e.message);
                }
                // Keep alive
                do {
                    bytesReceived = recv(sd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
                    this->logger->debug("Client sending " + std::to_string(bytesReceived) + " bytes.");
                } while (bytesReceived > 0);

                shutdown(sd, SHUT_RD);
                close(sd);
                client_socket[i] = 0;
            }
        }
    }
}


void Server::runSSL() {
    this->logger->info("Setting up SSL socket for port: " + std::to_string(this->portno));
    // SELECT
    fd_set readfds;
    int max_clients = 30;
    int client_socket[max_clients];
    int max_sd;
    int client;
    int sd;

    for (int i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    const SSL_METHOD *method;
    SSL_CTX *ctx;
    method = SSLv23_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        this->logger->error("Unable to create SSL context");
    }

    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert and chain */
    // Key
    const char *keyFile = config->sslPortmap[portno]["key"].c_str();
    if (SSL_CTX_use_PrivateKey_file(ctx, keyFile, SSL_FILETYPE_PEM) <= 0) {
        throw (ConfigException("Unable to read keyfile " + std::string(keyFile)));
    }

    // Cert
    const char *certFile = this->config->sslPortmap[this->portno]["cert"].c_str();
    if (SSL_CTX_use_certificate_file(ctx, certFile, SSL_FILETYPE_PEM) <= 0) {
        throw (ConfigException("Unable to read certfile " + std::string(certFile)));
    }

    int master_socket = makeMasterSocket(this->logger);
    struct sockaddr_in serv_addr = makeServerAddr(this->portno, master_socket, this->logger);
    int addrlen = sizeof(serv_addr);

    while(true) {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            this->logger->error("Select failed");
            throw SocketError();
        }
        if (FD_ISSET(master_socket, &readfds)) {
            client = accept(master_socket, (struct sockaddr*)&serv_addr, (socklen_t*)&addrlen);
            if (client < 0) {
                this->logger->error("Failed to accept new socket");
                throw SocketError();
            }
            for (int i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = client;
                    break;
                }
            }
        }

        this->logger->debug("Reading on SSL socket");
        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds)) {
                SSL *ssl = SSL_new(ctx);
                SSL_set_fd(ssl, sd);
                if (SSL_accept(ssl) <= 0) {
                    ERR_print_errors_fp(stderr);
                }
                try {
                    std::string headers = this->readSSLHeaders(ssl);
                    Request *request = new Request(headers, inet_ntoa(serv_addr.sin_addr), true, this->config, this->logger);
                    std::string response = request->getResponse();
                    SSL_write(ssl, response.c_str(), response.length());
                } catch (std::out_of_range) {
                    // this->sendError(400, sd);
                    this->logger->error("Bad request");
                } catch (RequestHeaderFieldTooLarge& e) {
                    // this->sendError(431, sd);
                    this->logger->warning(e.message);
                } catch (CouldNotParseHeaders& e) {
                    // this->sendError(500, sd);
                    this->logger->error(e.message);
                } catch (SocketError& e) {
                    // this->sendError(500, sd);
                    this->logger->error(e.message);
                }

                SSL_free(ssl);
                shutdown(sd, SHUT_RD);
                close(sd);
                client_socket[i] = 0;
            }
        }
    }
    SSL_CTX_free(ctx);
    EVP_cleanup();
}


void Server::run() {
    bool sslActive = this->config->sslPortmap[this->portno]["on"] == "true" ? true : false;
    if (sslActive) {
        this->runSSL();
    }
    else {
        this->runPlain();
    }
}


void Server::sendReply(std::string replyMessage, int sockfd) {

    int outMessageLen = send(sockfd, replyMessage.c_str(), replyMessage.length(), MSG_NOSIGNAL);
    this->logger->debug("Server::sendReply outMessageLen: " + std::to_string(outMessageLen));
    this->logger->debug("Server::sendReply replyMessage: " + replyMessage);

    if (outMessageLen < 0) {
        this->logger->error("Error: Writing to socket");
    }
}

std::string getErrorMessage(int status) {
    std::string errorMessage;
    switch (status) {
        case 431:
            errorMessage = "431 Request Header Fields Too Large";
            break;
    }
    return errorMessage;
}

void Server::sendError(int status, int sockfd) {

    std::string header = "HTTP/1.0 ";
    header += getErrorMessage(status);;
    header += "\n\r\n";

    int errorMessageLen = send(sockfd, header.c_str(), header.length(), MSG_NOSIGNAL);

    this->logger->debug("Server::sendError errorMessageLen: " + std::to_string(errorMessageLen));
    this->logger->debug("Server::sendError replyMessage: " + header);

    if (errorMessageLen < 0) {
        this->logger->error("Error: Writing to socket");
    }
}
