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
 #include <errno.h> // errno
#include <csignal> // signal
#include <string.h> //strerror

#include "../client/Client.hpp" // client class
#include "../pollset/Pollset.hpp" // pollset class

class Server {

    private:
        int port;
        int socket_fd;
        std::string password;
        std::vector<Client> clients;
        Pollset pollset;
        static bool signals;

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
        int getPortNumber(void) const;
        std::string getServerPassword(void) const;
        Client *getClientByFd(int fd_to_find);

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
        void receiveData(size_t &index);

        /* DISCONNECT CLIENT */
        void disconnectClient(size_t index);

        /* THROW + SYSTEM ERROR MESSAGE */
        void throwSystemError(const std::string &msg) const;

        /* SIGNAL HANDLER FUNCTION */
        static void signalHandler(int sig);
    };

#endif
