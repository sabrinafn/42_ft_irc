#include "../includes/client/Client.hpp"

/* CONSTRUCTOR */
Client::Client(void) : fd(0) {}

/* COPY CONSTRUCTOR */
Client::Client(const Client &other) { *this = other; }

/* OPERATORS */
Client &Client::operator=(const Client &other) {
    if (this != &other) {
        this->fd = other.fd;
    }
    return *this;
}

/* DESTRUCTOR */
Client::~Client(void) {}

/* SETTERS */
void Client::setFd(int other) {
    this->fd = other;
}

/* GETTERS */
int Client::getFd(void) {
    return this->fd;
}
