#include "../includes/client/Client.hpp"
#include <iostream>
#include <unistd.h>

/* CONSTRUCTOR */
Client::Client(void) : fd(0), buffer(""), last_activity(-1), ping_sent(false), 
    last_ping_sent(-1), state(UNREGISTERED), nickname(""), username(""), realname("") {}

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
        this->state = other.state;
        this->nickname = other.nickname;
        this->username = other.username;
        this->realname = other.realname;
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

void Client::setState(ClientState state) {
    this->state = state;
}

void Client::setNickname(const std::string& nick) {
    this->nickname = nick;
}

void Client::setUsername(const std::string& user) {
    this->username = user;
}

void Client::setRealname(const std::string& real) {
    this->realname = real;
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

ClientState Client::getState() const {
    return this->state;
}

const std::string& Client::getNickname() const {
    return this->nickname;
}

const std::string& Client::getUsername() const {
    return this->username;
}

const std::string& Client::getRealname() const {
    return this->realname;
}

std::string Client::getPrefix() const {
    std::string prefix = this->nickname + "!" + this->realname + "@";
    return prefix;
}

/* APPEND MORE DATA TO CURRENT BUFFER DATA */
void Client::appendData(std::string other) {
    this->buffer += other;
}

/* envia uma mensagem para cada cliente */
void Client::sendRawMessage(const std::string& message) {
    std::string msg = message + "\r\n";
    ssize_t ret = write(fd, msg.c_str(), msg.size());
    if (ret == -1) {
        std::cerr << "ERROR: falha ao enviar para " << nickname
                  << " (fd=" << this->fd << ")" << std::endl;
    } else {
        std::cout << "DEBUG: enviado para " << nickname
                  << " (fd=" << this->fd << ")"
                  << " -> " << msg << std::endl;
    }
}

