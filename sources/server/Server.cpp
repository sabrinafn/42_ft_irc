#include "../includes/server/Server.hpp"

bool Server::signals = false;

/* CONSTRUCTOR */
Server::Server(void) : port(-1), socket_fd(-1), password(""),
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
    while (!Server::signals) {
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
        perror("socket");
        throw std::runtime_error("Can't create a socket.");
    }

    // Allow address reuse (prevents "Address already in use" error)
    int opt = 1;
    if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(this->socket_fd);
        throw std::runtime_error("setsockopt not allowing address reuse.");
    }

    /* Setar O_NONBLOCK com fcntl() */
    this->setNonBlocking(this->socket_fd);

    // bind socket to a IP/port
    std::cout << "Binding server socket to ip address" << std::endl;
    if (bind(this->socket_fd, (struct sockaddr *)&hint, sizeof(hint)) == -1 ) {
        perror("bind");
        close(this->socket_fd);
        throw std::runtime_error("Can't bind to IP/port.");
    }

    // mark socket to start listening
    std::cout << "Marking socket to start listening" << std::endl;
    if (listen(this->socket_fd, SOMAXCONN) == -1) {
        close(this->socket_fd);
        perror("listen");
        throw std::runtime_error("Can't mark socket to start listening.");
   }

    // CREATING POLL STRUCT FOR SERVER AND ADDING TO THE POLLFD STRUCT
    this->pollFds.add(this->socket_fd);
}


/* MONITORING FOR ACTIVITY ON FDS */
void Server::monitorConnections(void) {
    // MONITORING FDS AND WAITING FOR EVENTS TO HAPPEN
    if (this->pollFds.poll() == -1 && !Server::signals) {
        perror("poll");
        close(this->socket_fd);
        throw std::runtime_error("poll() can't monitor fds");
    }
    std::cout << "poll waiting for an event to happen" << std::endl;
    // checking all fds
    for (int i = 0; i < pollFds.getSize(); i++) {
        // CHECK IF THIS CURRENT SOCKET RECEIVED INPUT
        struct pollfd current = this->pollFds.getPollFd(i);
        if (current.revents & POLLIN) {
            // CHECK IF ANY EVENTS HAPPENED ON SERVER SOCKET
			std::cout << "Client with fd [" << current.fd << "] connected" << std::endl;
            if (current.fd == this->socket_fd) {
                // accept a new client
                this->acceptClient();
            }
			else {
                // receive data for client that is already registered
                this->receiveData(i);
            }
        }
        else if (current.revents & POLLHUP || current.revents & POLLERR) {
            // i = index i in pollfds[i], not the fd
            this->disconnectClient(i);
            --i;
        }

    }
}

/* SET SOCKETS AS NON BLOCKING */
void Server::setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        if (socket != this->socket_fd)
            close(socket);
        close(this->socket_fd);
        throw std::runtime_error("Erro ao obter flags do socket");
    }
    flags |= O_NONBLOCK;
    if (fcntl(socket, F_SETFL, flags) == -1) {
        perror("fcntl");
        if (socket != this->socket_fd)
            close(socket);
        close(this->socket_fd);
        throw std::runtime_error("Erro ao setar O_NONBLOCK");
    }
}

/* ACCEPT A NEW CLIENT */
void Server::acceptClient(void) {
    // accept a new client
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(this->socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) {
        perror("accept");
        std::cerr << "Can't connect to client" << std::endl;
        return;
    }

    /* Setar O_NONBLOCK with fcntl() */
    this->setNonBlocking(client_fd);

    // CREATING POLL STRUCT FOR CURRENT CLIENT AND ADDING TO THE POLLFD STRUCT
    this->pollFds.add(client_fd);

    Client client;
    client.setFd(client_fd);
    this->clients_fd.push_back(client);
}

/* RECEIVE DATA FROM REGISTERED CLIENT */
void Server::receiveData(int &index) {
    char buffer[1024];
    struct pollfd current = this->pollFds.getPollFd(index);
    ssize_t bytes_read = recv(current.fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read < 0) {
        perror("recv");
        std::cerr << "Can't read, recv failed" << std::endl;
        this->disconnectClient(index);
        --index; // fix index after erase
        return;
    }
    else if (bytes_read == 0) {
        std::cerr << "client disconnected" << std::endl;
        this->disconnectClient(index);
        --index; // fix index after erase
        return;
    }
    else {
        buffer[bytes_read] = '\0';
        // print data received and stored in buffer
        int idx_cli = findClientByFd(current.fd);
        if (idx_cli == -1) {
            throw std::invalid_argument("Client fd not found");
        }
        std::string buf = buffer;
        this->clients_fd[idx_cli].appendData(buf);
        std::cout << "Client fd [" << this->clients_fd[idx_cli].getFd() << "]"
                  << " data: '" << this->clients_fd[idx_cli].getData() << "'" << std::endl;
    }
}

/* DISCONNECT CLIENT */
void Server::disconnectClient(int index) {
    struct pollfd current = this->pollFds.getPollFd(index);
    close(current.fd);
    this->pollFds.remove(index);
}

/* FIND CLIENT BY FD */
int Server::findClientByFd(int fd_to_find) {

    std::vector<Client>::iterator it = clients_fd.begin();
    unsigned long i;
    for (i = 0; i < clients_fd.size(); i++) {
        if ((*it).getFd() == fd_to_find)
            return static_cast<int>(i);
        it++;
    }
    return -1;
}

/* SIGNAL HANDLER FUNCTION */
void Server::signalHandler(int sig) {
    std::cout << "Program terminated with '" << sig << "'" << std::endl;
    (void)sig;
    Server::signals = true;
}

