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
        std::vector<int> clients_fd;
        std::string data_received;

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
        void connectToClient(void);
};

#endif
