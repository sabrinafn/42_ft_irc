#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>

class Server {

    private:

    public:
        /* CONSTRUCTORS */
        Server(void);
        Server(const Server &other);

        /* DESTRUCTOR */
        ~Server(void);

        /* OPERATOR OVERLOADING */
        Server &operator=(const Server &other);
}

#endif