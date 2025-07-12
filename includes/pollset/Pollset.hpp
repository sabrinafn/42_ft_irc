#ifndef POLLSET_HPP
#define POLLSET_HPP

#include <vector> // std::vector
#include <poll.h> // poll

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

};

#endif
