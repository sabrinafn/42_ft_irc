#include "../includes/client/Client.hpp"

/* CONSTRUCTOR */
Client::Client(void) : fd(0), data(""), last_activity(0) {}

/* COPY CONSTRUCTOR */
Client::Client(const Client &other) { *this = other; }

/* OPERATORS */
Client &Client::operator=(const Client &other) {
    if (this != &other) {
        this->fd = other.fd;
        this->data = other.data;
        this->last_activity = other.last_activity;
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
    this->data = other;
}

void Client::setLastActivity(time_t time) {
    this->last_activity = time;
}

/* GETTERS */
int Client::getFd(void) {
    return this->fd;
}

std::string Client::getData(void) {
    return this->data;
}

time_t Client::getLastActivity(void) {
    return this->last_activity;
}

/* APPEND MORE DATA TO CURRENT BUFFER DATA */
void Client::appendData(std::string other) {
    this->data += other;
}
