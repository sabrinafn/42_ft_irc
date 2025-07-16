#ifndef POLLSET_HPP
#define POLLSET_HPP

#include <vector> // std::vector
#include <poll.h> // poll
#include <iostream> // std::cout std::cerr

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
        //struct pollfd operator[](int index);

        /* DESTRUCTOR */
        ~Pollset(void);

        /* ADD FD TO THE POLL*/
        void add(int fd);

        /* REMOVE FD FROM THE POLL */
        void remove(int fd);

        /* POLL METHOD TO WRAP POLL FUNCTION CALL */
        int poll(void);

        /* GETTTERS */
        int getSize(void);
        struct pollfd &getPollFd(int index);

    };

#endif
