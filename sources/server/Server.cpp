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
    acceptClients();

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

   // server socket here
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

    struct pollfd serverPollFd;
    serverPollFd.fd = this->socket_fd; //-> add the server socket to the pollfd
    serverPollFd.events = POLLIN; //-> set the event to POLLIN for reading data
    serverPollFd.revents = 0; //-> set the revents to 0

    this->pollFds.push_back(serverPollFd); //-> add the server socket to the pollfd

}


/* CONNECT TO CLIENT */
void Server::acceptClients(void) {

    while (true) {
        
        // waiting for an event to happen
        std::cout << "poll waiting for an event to happen" << std::endl;
        if((poll(this->pollFds.data(), this->pollFds.size(), -1) == -1)) {
            std::cerr << "Can't listen" << std::endl;
            return;
        }
        // checking all fds
        for (int i = 0; i < (int)this->pollFds.size(); i++) {
            if (this->pollFds[i].revents & POLLIN)//-> check if there is data to read
			{
                std::cout << "Client with fd [" << i << "] connected" << std::endl;
				if (this->pollFds[i].fd == this->socket_fd) {
                    // accept a new client
                    // accept(socket fd, socket struct new fd, socket struct size)
                    struct sockaddr_in client_addr;

                    // data type to store size of socket structures
                    socklen_t client_addr_len = sizeof(client_addr);

                    int client_fd = accept(this->socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
                    if (client_fd < 0) {
                        std::cerr << "Can't connect to client" << std::endl;
                        continue ;
                    }
                
                    /* Setar each client flags as O_NONBLOCK with fcntl() */
                    int flags = fcntl(client_fd, F_GETFL, 0);
                    if (flags == -1) {
                        std::cerr << "Error to get socket flags\n";
                        close(client_fd);
                        return ;
                    }
                    flags |= O_NONBLOCK;
                    if (fcntl(client_fd, F_SETFL, flags) == -1) {
                        std::cerr << "Error to set O_NONBLOCK\n";
                        close(client_fd);
                        return ;
                    }
                
                    struct pollfd client_poll_fd;
                    client_poll_fd.fd = client_fd; // add the client socket to the pollfd
                    client_poll_fd.events = POLLIN; // set the event to POLLIN for reading data
                    client_poll_fd.revents = 0; // set the revents to 0
                

                    this->pollFds.push_back(client_poll_fd); //-> add the server socket to the pollfd
                }
				else {
                    // receive data for client that is already registered
                    char buffer[1024];
                    ssize_t bytes_read = recv(this->pollFds[i].fd, buffer, sizeof(buffer) - 1, 0);
                    if (bytes_read < 0) {
                        std::cerr << "Error to use read from fd with recv" << std::endl;
                        close(this->pollFds[i].fd);
                        this->pollFds.erase(this->pollFds.begin() + i);
                        --i; // fix index after erase
                        continue;
                    }
                    if (bytes_read == 0) {
                        std::cerr << "fd read completely" << std::endl;
                        close(this->pollFds[i].fd);
                        this->pollFds.erase(this->pollFds.begin() + i);
                        --i; // fix index after erase
                        continue;
                    }
                    buffer[bytes_read] = '\0';
                
                    // print data received and stored in buffer
                    std::cout << "Client [" << i << "]" << " data: '" << buffer << "'" << std::endl;
                }
            }
        }
    }
}
