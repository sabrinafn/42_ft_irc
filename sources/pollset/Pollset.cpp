#include "../../includes/pollset/Pollset.hpp"

/* CONSTRUCTOR */
Pollset::Pollset(void) : fd(0) {}

/* COPY CONSTRUCTOR */
Pollset::Pollset(const Pollset &other) { *this = other; }

/* OPERATORS */
Pollset &Pollset::operator=(const Pollset &other) {
    if (this != &other) {
        this->fd = other.fd;
    }
    return *this;
}

/* DESTRUCTOR */
Pollset::~Pollset(void) {}

/* ADD FD TO THE POLL*/
void Pollset::add(int fd) {
    struct pollfd poll_fd;
    poll_fd.fd = fd; //-> add the server socket to the pollfd
    poll_fd.events = POLLIN; //-> set the event to POLLIN for reading data
    poll_fd.revents = 0; //-> set the revents to 0

    this->fd.push_back(poll_fd); //-> add the server socket to the pollfd
}

/* REMOVE FD FROM THE POLL */
void Pollset::remove(int fd) {
    std::vector<pollfd>::iterator pos = this->fd.begin() + fd;
    this->fd.erase(pos);
}

/* POLL METHOD TO WRAP POLL FUNCTION CALL */
int Pollset::poll(void) {
    // MONITORING FDS AND WAITING FOR EVENTS TO HAPPEN
    return ::poll(this->fd.data(), this->fd.size(), -1);
}

/* GETTERS */
size_t Pollset::getSize(void) const {
    return this->fd.size();
}

struct pollfd &Pollset::getPollFd(int index) {
    return this->fd[index];
}

/* CLEAR POLLFD VECTOR */
void Pollset::clear(void) {
    // close fds in struct pollfd
    for (size_t i = 0; i < this->fd.size(); i++) {
        close(this->fd[i].fd);
        this->fd.erase(this->fd.begin() + i);
    }
}
