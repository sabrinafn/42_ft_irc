#include "Client.hpp"

/* CONSTRUCTORS */
Client::Client(void) {}

Client::Client(const Client &other) {
    *this = other;
}

/* DESTRUCTOR */
Client::~Client(void) {}

/* OPERATOR OVERLOADING */
Client& Client::operator=(const Client &other) {
    if (this != &other) {
        (void)other;
    }
    return this;
}