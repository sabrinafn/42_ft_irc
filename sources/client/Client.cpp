#include "../includes/client/Client.hpp"

/* CONSTRUCTOR */
Client::Client(void) : fd(0), buffer("") {}

/* COPY CONSTRUCTOR */
Client::Client(const Client &other) { *this = other; }

/* OPERATORS */
Client &Client::operator=(const Client &other) {
    if (this != &other) {
        this->fd = other.fd;
        this->buffer = other.buffer;
    }
    return *this;
}

/* DESTRUCTOR */
Client::~Client(void) {}

/* SETTERS */
void Client::setFd(int other) {
    this->fd = other;
}

void Client::setData(std::string other) {
    this->buffer = other;
}

/* GETTERS */
int Client::getFd(void) const {
    return this->fd;
}

std::string Client::getData(void) const {
    return this->buffer;
}

/* APPEND MORE DATA TO CURRENT BUFFER DATA */
void Client::appendData(std::string other) {
    this->buffer += other;
}
