#include "../includes/server/Server.hpp"

/* CONSTRUCTOR */
Server::Server(void) : port(0), socket_fd(0), password(""),
    clients_fd(0, 0), pollFds(0, 0) {}

/* COPY CONSTRUCTOR */
Server::Server(const Server &other) { *this = other;}

/* OPERATORS */
Server &Server::operator=(const Server &other) {
    if (this != &other) {
        this->port = other.port;
        this->socket_fd = other.socket_fd;
        this->password = other.password;
        this->clients_fd = other.clients_fd;
        this->pollFds = other.pollFds;
    }
    return *this;
}

/* DESTRUCTOR */
Server::~Server(void) {}

/* SETTERS */
void Server::setPortNumber(int other) {
    this->port = other;
}

void Server::setServerPassword(std::string other) {
    this->password = other;
}

/* GETTERS */
int Server::getPortNumber(void) {
    return this->port;
}

std::string Server::getServerPassword(void) {
    return this->password;
}

/* INIT SERVER */
void Server::initServer(void) {

    createSocket();

    // use poll inside loop to connect and read from clients
    acceptClients();

}

/* CREATE SOCKET */
void Server::createSocket(void) {
    
    sockaddr_in hint;
    hint.sin_family = AF_INET; // IPV4
    hint.sin_port = htons(this->port); // convert number to network byte order
    hint.sin_addr.s_addr = INADDR_ANY; // set the address to any local machine address
    //inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    // create socket
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    //int listening_socket = socket(AF_INET, SOCK_STREAM, 0); // replace by line above
    if (this->socket_fd == -1) {
        std::cerr << "Can't create a socket." << std::endl;
        return ;
    }

    // Allow address reuse (prevents "Address already in use" error)
    int opt = 1;
    if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt error" << std::endl;
        close(this->socket_fd);
        return;
    }

   /***********/

   /* Setar O_NONBLOCK com fcntl() */
   int flags = fcntl(this->socket_fd, F_GETFL, 0);
   if (flags == -1) {
       std::cerr << "Erro ao obter flags do socket\n";
       close(this->socket_fd);
       return ;
   }
   flags |= O_NONBLOCK;
   if (fcntl(this->socket_fd, F_SETFL, flags) == -1) {
       std::cerr << "Erro ao setar O_NONBLOCK\n";
       close(this->socket_fd);
       return ;
   }


   /***********/


   // bind socket to a IP/port
   if (bind(this->socket_fd, (struct sockaddr *)&hint, sizeof(hint)) == -1 ) {
       std::cerr << "Can't bind to IP/port";
       return;
   }

   // mark socket to start listening
   if (listen(this->socket_fd, SOMAXCONN) == -1) {
       std::cerr << "Can't listen" << std::endl;
       return;
   }

    struct pollfd serverPollFd;
    serverPollFd.fd = this->socket_fd; //-> add the server socket to the pollfd
    serverPollFd.events = POLLIN; //-> set the event to POLLIN for reading data
    serverPollFd.revents = 0; //-> set the revents to 0

    this->pollFds.push_back(serverPollFd); //-> add the server socket to the pollfd

}


/* CONNECT TO CLIENT */
void Server::acceptClients(void) {

    while (true) {

        // poll = waits for some events on a fd

        struct pollfd = NULL;
        int poll_fd = poll(fds, 1024, -1);
        
        for (int i = 0; i < 1024; i++) {
            // accept(socket fd, socket struct new fd, socket struct size)
            struct sockaddr_in client_addr;


            // data type to store size of socket structures
            socklen_t client_addr_len = sizeof(client_addr);


            int client_one = accept(this->socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
            pollfd->fd.push_back(client_one);
            if (pollfd->fd[i] < 0) {
                std::cerr << "Can't connect to client" << std::endl;
                close(this->socket_fd);
                close(pollfd->fd[i]);
                return ;
            }
            char buffer[1024];
            ssize_t bytes_read = read(pollfd->fd[i], buffer, sizeof(buffer) - 1);
            if (bytes_read < 0) {
                std::perror("read");
                // Close sockets
                close(pollfd->fd[i]);
                close(pollfd->fd[i]);
                return ;
            }
            buffer[bytes_read] = '\0';

            // print message received and stored in buffer
            std::cout << "Received message: " << buffer << std::endl;


            close(pollfd->fd[i]);
            close(this->socket_fd);
        }
    }
}

