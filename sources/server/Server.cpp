#include "../includes/server/Server.hpp"

/* CONSTRUCTOR */
Server::Server(void) : port(0), socket_fd(0), password("") {}

/* COPY CONSTRUCTOR */
Server::Server(const Server &other) { *this = other;}

/* OPERATORS */
Server &Server::operator=(const Server &other) {
    if (this != &other) {
        this->port = other.port;
        this->socket_fd = other.socket_fd;
    }
    return *this;
}

/* DESTRUCTOR */
Server::~Server(void) {}

void Server::setPortNumber(int other) {
    this->port = other;
}

void Server::setServerPassword(std::string other) {
    this->password = other;
}

int Server::getPortNumber(void) {
    return this->port;
}

std::string Server::getServerPassword(void) {
    return this->password;
}
