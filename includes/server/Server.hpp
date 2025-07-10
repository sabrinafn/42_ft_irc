#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

#include <vector>
#include <string>

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
