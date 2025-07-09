#include "../includes/server/Server.hpp"

/* CONSTRUCTOR */
Server::Server(void) : port(0), server_socket_fd(0) {}

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
    int listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_socket == -1) {
        std::cerr << "Can't create a socket." << std::endl;
        return ;
    }

    // bind socket to a IP/port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000); // random port within the range of available ports 49152–65535
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(listening_socket, AF_INET, &hint, sizeof(hint)) == -1 ) {
        std::cerr << "Can't bind to IP/port";
        return;
    }
}