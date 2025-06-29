#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Client {

    private:

    public:
        /* CONSTRUCTORS */
        Client(void);
        Client(const Client &other);

        /* DESTRUCTOR */
        ~Client(void);

        /* OPERATOR OVERLOADING */
        Client &operator=(const Client &other);

}

#endif