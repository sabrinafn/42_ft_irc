#include "../includes/ft_irc.hpp"

/* CONSTRUCTOR */
Client::Client(void)
    : fd(0),
      buffer(""),
      last_activity(-1),
      ping_sent(false),
      last_ping_sent(-1),
      state(UNREGISTERED),
      nickname(""),
      username(""),
      realname("") {
}

/* COPY CONSTRUCTOR */
Client::Client(const Client& other) {
    *this = other;
}

/* OPERATORS */
Client& Client::operator=(const Client& other) {
    if (this != &other) {
        this->fd             = other.fd;
        this->buffer         = other.buffer;
        this->last_activity  = other.last_activity;
        this->ping_sent      = other.ping_sent;
        this->last_ping_sent = other.last_ping_sent;
        this->state          = other.state;
        this->nickname       = other.nickname;
        this->username       = other.username;
        this->realname       = other.realname;
    }
    return *this;
}

/* DESTRUCTOR */
Client::~Client(void) {
}

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

/* SEND IRC REPLY MESSAGE TO EACH CLIENT */
void Client::sendReply(const std::string& message) {
    std::string msg = message + "\r\n";
    ssize_t     ret = send(fd, msg.c_str(), msg.size(), 0);
    if (ret == -1) {
    int err = errno;
    std::stringstream ss;
    ss << "send() failed for client [" << this->fd 
       << "]: " << strerror(err) << " (errno=" << err << ")";
    logError(ss.str());
} else {
        std::stringstream ss;
        ss << "Reply message sent to client [" << this->fd << "]" << " : " << msg;
        logInfo(ss.str());
    }
}

/* SEND WELCOME MESSAGES AFTER REGISTRATION */
void Client::sendWelcomeMessages(void) {
    this->sendReply(RPL_WELCOME(this->nickname, this->realname));
    this->sendReply(RPL_YOURHOST(this->nickname));
    std::string startup_time = "2001-01-01 01:01:01"; // criar funcao pra pegar data atual
    this->sendReply(RPL_CREATED(this->nickname, startup_time));
    this->sendReply(RPL_MYINFO(this->nickname, "o", "o"));

    std::stringstream ss;
    ss << "Client [" << this->fd << "] (" << this->nickname << ") is now registered!";
    logInfo(ss.str());
}
