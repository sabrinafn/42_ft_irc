#include "../../includes/pollset/Pollset.hpp"

/* CONSTRUCTOR */
Pollset::Pollset(void) {}

/* COPY CONSTRUCTOR */
Pollset::Pollset(const Pollset &other) { *this = other; }

/* OPERATORS */
Pollset &Pollset::operator=(const Pollset &other) {
    if (this != &other) {
        (void)other;
    }
    return *this;
}

/* DESTRUCTOR */
Pollset::~Pollset(void) {}

/* ADD FD TO THE POLL*/
void Pollset::add(int fd) {

}

/* REMOVE FD FROM THE POLL */
void Pollset::remove(int fd) {
    
}