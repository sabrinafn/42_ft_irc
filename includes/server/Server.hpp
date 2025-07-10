#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

class Server {

    private:
        int port;
        int socket_fd;
        std::string password;

    public:
        /* CONSTRUCTOR */
        Server(void);

        /* COPY CONSTRUCTOR */
        Server(const Server &other);

        /* OPERATORS */
        Server &operator=(const Server &other);

        /* DESTRUCTOR */
        ~Server(void);

        /* SETTERS */
        void setPortNumber(int other);
        void setServerPassword(std::string other);

        /* GETTERS */
        int getPortNumber(void);
        std::string getServerPassword(void);
};

#endif
