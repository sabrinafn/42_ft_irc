#include "../includes/ft_irc.hpp"

Pollset::Pollset(void) : fd(0) {
}

Pollset::Pollset(const Pollset &other) {
    *this = other;
}

Pollset &Pollset::operator=(const Pollset &other) {
    if (this != &other) {
        this->fd = other.fd;
    }
    return *this;
}


Pollset::~Pollset(void) {
}

void Pollset::add(int fd) {
    struct pollfd poll_fd;
    poll_fd.fd      = fd;     
    poll_fd.events  = POLLIN; 
    poll_fd.revents = 0;     

    this->fd.push_back(poll_fd); 
}

void Pollset::remove(int index) {
    std::vector<pollfd>::iterator pos = this->fd.begin() + index;
    this->fd.erase(pos);
}

int Pollset::poll(void) {
    return ::poll(this->fd.data(), this->fd.size(), 1000);
}

size_t Pollset::getSize(void) const {
    return this->fd.size();
}

struct pollfd &Pollset::getPollFd(int index) {
    return this->fd[index];
}

const std::vector<pollfd> &Pollset::getPollfds() const {
    return this->fd;
}

void Pollset::clear(void) {
    for (size_t i = 0; i < this->fd.size(); i++) {
        close(this->fd[i].fd);
        this->fd.erase(this->fd.begin() + i);
    }
}
