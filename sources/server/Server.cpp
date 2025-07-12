#include "../includes/server/Server.hpp"

/* CONSTRUCTOR */
Server::Server(void) : port(0), socket_fd(0), password(""),
    clients_fd(), pollFds() {}

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
    while (true) {
        monitorConnections();
    }
}

/* CREATE SOCKET */
void Server::createSocket(void) {
    
    sockaddr_in hint;
    hint.sin_family = AF_INET; // IPV4
    hint.sin_port = htons(this->port); // convert number to network byte order
    hint.sin_addr.s_addr = INADDR_ANY; // set the address to any local machine address

    // create socket
    std::cout << "Creating server socket..." << std::endl;
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
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

   /* Setar O_NONBLOCK com fcntl() */
   this->setNonBlocking(this->socket_fd);

   // bind socket to a IP/port
   std::cout << "Binding server socket to ip address" << std::endl;
   if (bind(this->socket_fd, (struct sockaddr *)&hint, sizeof(hint)) == -1 ) {
       std::cerr << "Can't bind to IP/port";
       return;
   }

   // mark socket to start listening
   std::cout << "Marking socket to start listening" << std::endl;
   if (listen(this->socket_fd, SOMAXCONN) == -1) {
       std::cerr << "Can't listen" << std::endl;
       return;
   }

    // CREATING POLL STRUCT FOR SERVER AND ADDING TO THE POLLFD STRUCT
    this->pollFds.add(this->socket_fd);
}


/* MONITORING FOR ACTIVITY ON FDS */
void Server::monitorConnections(void) {
    // MONITORING FDS AND WAITING FOR EVENTS TO HAPPEN
    std::cout << "poll waiting for an event to happen" << std::endl;
    if (this->pollFds.poll(-1) == -1) {
        return ;
    }
    // checking all fds
    for (int i = 0; i < pollFds.getSize(); i++) {
        // CHECK IF THIS CURRENT SOCKET RECEIVED INPUT
        if (this->pollFds[i].revents & POLLIN)
		{
            // CHECK IF ANY EVENTS HAPPENED ON SERVER SOCKET
            std::cout << "Client with fd [" << i << "] connected" << std::endl;
			if (this->pollFds[i].fd == this->socket_fd) {
                // accept a new client
                this->acceptClient();
            }
			else {
                // receive data for client that is already registered
                this->receiveData(i);
            }
        }
    }
}

/* SET SOCKETS AS NON BLOCKING */
void Server::setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
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
}

/* ACCEPT A NEW CLIENT */
void Server::acceptClient(void) {
    // accept a new client
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(this->socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) {
        std::cerr << "Can't connect to client" << std::endl;
        return;
    }

    /* Setar O_NONBLOCK with fcntl() */
    this->setNonBlocking(client_fd);

    // CREATING POLL STRUCT FOR CURRENT CLIENT AND ADDING TO THE POLLFD STRUCT
    this->pollFds.add(client_fd);
}

/* RECEIVE DATA FROM REGISTERED CLIENT */
void Server::receiveData(int &fd) {
    char buffer[1024];
    ssize_t bytes_read = recv(this->pollFds[fd].fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read < 0) {
        std::cerr << "Error to use read from fd with recv" << std::endl;
        close(this->pollFds[fd].fd);
        pollFds.remove(fd);
        --fd; // fix index after erase
        return;
    }
    if (bytes_read == 0) {
        std::cerr << "fd read completely" << std::endl;
        close(this->pollFds[fd].fd);
        pollFds.remove(fd);
        --fd; // fix index after erase
        return;
    }
    buffer[bytes_read] = '\0';

    // print data received and stored in buffer
    std::cout << "Client [" << fd << "]" << " data: '" << buffer << "'" << std::endl;
}
