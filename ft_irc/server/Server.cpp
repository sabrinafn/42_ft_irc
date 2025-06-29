#include "Server.hpp"

/* CONSTRUCTORS */
Server::Server(void) {}

Server::Server(const Server &other) {
    *this = other;
}

/* DESTRUCTOR */
Server::~Server(void) {}

/* OPERATOR OVERLOADING */
Server& Server::operator=(const Server &other) {
    if (this != &other) {
        (void)other;
    }
    return *this;
}