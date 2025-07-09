#include "../includes/server/Server.hpp"

/* CONSTRUCTOR */
Server::Server(void) : port(54000), server_socket_fd(0), clients_fd(0, 0) {}

/* COPY CONSTRUCTOR */
Server::Server(const Server &other) { *this = other;}

/* OPERATORS */
Server &Server::operator=(const Server &other) {
    if (this != &other) {
        this->port = other.port;
        this->server_socket_fd = other.server_socket_fd;
    }
    return *this;
}

/* DESTRUCTOR */
Server::~Server(void) {}


/* CREATE SOCKET */
void Server::createSocket(void) {

    // create socket
     this->server_socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
    //int listening_socket = socket(AF_INET, SOCK_STREAM, 0); // replace by line above
    if (this->server_socket_fd == -1) {
        std::cerr << "Can't create a socket." << std::endl;
        return ;
    }

    /***********/

    /* Setar O_NONBLOCK com fcntl() */
    int flags = fcntl(this->server_socket_fd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Erro ao obter flags do socket\n";
        close(this->server_socket_fd);
        return ;
    }
    flags |= O_NONBLOCK;
    if (fcntl(this->server_socket_fd, F_SETFL, flags) == -1) {
        std::cerr << "Erro ao setar O_NONBLOCK\n";
        close(this->server_socket_fd);
        return ;
    }

    /***********/

    // bind socket to a IP/port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(this->port);
    // hint.sin_port = htons(54000); // replace by line above
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(this->server_socket_fd, (struct sockaddr *)&hint, sizeof(hint)) == -1 ) {
        std::cerr << "Can't bind to IP/port";
        return;
    }

    // mark socket to start listening
    if (listen(this->server_socket_fd, SOMAXCONN) == -1) {
        std::cerr << "Can't listen" << std::endl;
        return;
    }



    // accept(socket fd, socket struct new fd, socket struct size)
    struct sockaddr_in client_addr;
    // data type to store size of socket structures
    socklen_t client_addr_len = sizeof(client_addr);
    int client_one = accept(this->server_socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    this->clients_fd.push_back(client_one);
    if (this->clients_fd[0] < 0) {
        std::cerr << "Can't connect to client" << std::endl;
        close(this->server_socket_fd);
        return ;
    }

}