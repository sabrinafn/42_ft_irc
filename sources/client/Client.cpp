#include "../includes/client/Client.hpp"

/* CONSTRUCTOR */
Client::Client(void) : fd(0), buffer(""), first_connection_time(std::time(0)), last_activity(std::time(0)) {}

/* COPY CONSTRUCTOR */
Client::Client(const Client &other) { *this = other; }

/* OPERATORS */
Client &Client::operator=(const Client &other) {
    if (this != &other) {
        this->fd = other.fd;
        this->buffer = other.buffer;
        this->first_connection_time = other.first_connection_time;
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
    this->buffer = other;
}


void Client::setFirstConnectionTime(time_t t) {
    first_connection_time = t;
}

void Client::setLastActivity(time_t t) {
    last_activity = t;
}

/* GETTERS */
int Client::getFd(void) const {
    return this->fd;
}

std::string Client::getData(void) const {
    return this->buffer;
}

time_t Client::getFirstConnectionTime() const {
    return first_connection_time;
}

time_t Client::getLastActivity() const {
    return last_activity;
}

/* APPEND MORE DATA TO CURRENT BUFFER DATA */
void Client::appendData(std::string other) {
    this->buffer += other;
}
