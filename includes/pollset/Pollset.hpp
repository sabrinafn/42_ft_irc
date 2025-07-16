
/*
 * Pollset.hpp
 *
 * Defines the Pollset class, which encapsulates a collection of pollfd structs
 * and provides methods for managing and polling multiple file descriptors.
 */
#ifndef POLLSET_HPP
#define POLLSET_HPP

#include <vector> // std::vector
#include <poll.h> // poll
#include <iostream> // std::cout std::cerr
#include <unistd.h> // close

class Pollset {

    private:
        std::vector<pollfd> fd;

    public:
        /* CONSTRUCTOR */
        Pollset(void);

        /* COPY CONSTRUCTOR */
        Pollset(const Pollset &other);

        /* OPERATORS */
        Pollset &operator=(const Pollset &other);

        /* DESTRUCTOR */
        ~Pollset(void);

        /* ADD FD TO THE POLL*/
        void add(int fd);

        /* REMOVE FD FROM THE POLL */
        void remove(int fd);

        /* POLL METHOD TO WRAP POLL FUNCTION CALL */
        int poll(void);

        /* GETTERS */
        size_t getSize(void) const;
        struct pollfd &getPollFd(int index);
        
        /* CLEAR POLLFD VECTOR */
        void clear(void);

    };

#endif
