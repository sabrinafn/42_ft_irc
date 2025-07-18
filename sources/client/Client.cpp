#include "../includes/client/Client.hpp"

/* CONSTRUCTOR */
Client::Client(void) : fd(0), buffer(""), last_activity(-1), ping_sent(false), last_ping_sent(-1) {}

/* COPY CONSTRUCTOR */
Client::Client(const Client &other) { *this = other; }

/* OPERATORS */
Client &Client::operator=(const Client &other) {
    if (this != &other) {
        this->fd = other.fd;
        this->buffer = other.buffer;
        this->last_activity = other.last_activity;
        this->ping_sent = other.ping_sent;
        this->last_ping_sent = other.last_ping_sent;
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

void Client::setLastActivity(time_t t) {
    last_activity = t;
}

void Client::setPingSent(bool set) {
    this->ping_sent = set;
}

void Client::setLastPingSent(time_t time) {
    this->last_ping_sent = time;
}

/* GETTERS */
int Client::getFd(void) const {
    return this->fd;
}

std::string Client::getData(void) const {
    return this->buffer;
}

time_t Client::getLastActivity() const {
    return last_activity;
}

bool Client::pingSent(void) {
    return this->ping_sent;
}

time_t Client::getLastPingSent(void) {
    return this->last_ping_sent;
}

/* APPEND MORE DATA TO CURRENT BUFFER DATA */
void Client::appendData(std::string other) {
    this->buffer += other;
}
