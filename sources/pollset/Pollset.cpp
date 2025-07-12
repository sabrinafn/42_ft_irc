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

struct pollfd Pollset::operator[](int index) {
    return this->fd[index];
}

/* DESTRUCTOR */
Pollset::~Pollset(void) {}

/* ADD FD TO THE POLL*/
void Pollset::add(int fd) {
    // 01. CREATING POLL STRUCT FOR SERVER
    struct pollfd poll_fd;
    poll_fd.fd = fd; //-> add the server socket to the pollfd
    poll_fd.events = POLLIN; //-> set the event to POLLIN for reading data
    poll_fd.revents = 0; //-> set the revents to 0

    // 02. ADDING SERVER POLL FD TO THE POLLFD STRUCT
    this->fd.push_back(poll_fd); //-> add the server socket to the pollfd
}

/* REMOVE FD FROM THE POLL */
void Pollset::remove(int fd) {
    std::vector<pollfd>::iterator pos = this->fd.begin() + fd;
    this->fd.erase(pos);
}

/* POLL METHOD TO WRAP POLL FUNCTION CALL */
int Pollset::poll(int timeout) {
    // 03. MONITORING FDS AND WAITING FOR EVENTS TO HAPPEN
    int result;
    result = ::poll(this->fd.data(), this->fd.size(), timeout);
    if (result == -1) {
        std::cerr << "Can't listen" << std::endl;
        //throw error
        return -1;
    }
    return result;
}

/* GETTTERS */
int Pollset::getSize(void) {
    return static_cast<int>(this->fd.size());
}
