#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream> // std::cout, std::cerr
#include <unistd.h> // read
//#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // htons, inet_pton
#include <sys/socket.h> // socket, bind, listen, accept
//#include <netdb.h>
#include <fcntl.h> // fcntl
#include <poll.h> // poll
#include <vector> // std::vector
#include <string> // std::string

#include "../client/Client.hpp" // client class

class Server {

    private:
        int port;
        int socket_fd;
        std::string password;
        std::vector<Client> clients_fd;
        std::vector<struct pollfd> pollFds;

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

        /* CONNECT TO CLIENT */
        void acceptClients(void);
};

#endif
