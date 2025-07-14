#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream> // std::cout, std::cerr
#include <unistd.h> // read
#include <arpa/inet.h> // htons, inet_pton
#include <sys/socket.h> // socket, bind, listen, accept
#include <fcntl.h> // fcntl
#include <poll.h> // poll
#include <vector> // std::vector
#include <string> // std::string
#include <cstdio> // perror

#include "../client/Client.hpp" // client class
#include "../pollset/Pollset.hpp" // pollset class

class Server {

    private:
        int port;
        int socket_fd;
        std::string password;
        std::vector<Client> clients_fd;
        Pollset pollFds;

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

        /* CREATE SOCKET */
        void createSocket(void);

        /* INIT SERVER */
        void initServer(void);

        /* MONITORING FOR ACTIVITY ON FDS */
        void monitorConnections(void);

        /* SET SOCKETS AS NON BLOCKING */
        void setNonBlocking(int socket);

        /* ACCEPT A NEW CLIENT */
        void acceptClient(void);

        /* RECEIVE DATA FROM REGISTERED CLIENT */
        void receiveData(int &index);

        /* DISCONNECT CLIENT */
        void disconnectClient(int index);

        /* FIND CLIENT BY FD */
        int findClientByFd(int fd_to_find);
    };

#endif
