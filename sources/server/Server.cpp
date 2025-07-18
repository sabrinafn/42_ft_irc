#include "../includes/server/Server.hpp"

bool Server::signals = false;

/* CONSTRUCTOR */
Server::Server(void) : port(-1), socket_fd(-1), password(""),
    clients(), pollset(), timeout_seconds(300), pong_timeout(15) {} // 300 for 5 min

Server::Server(int port, const std::string &password) : port(port), socket_fd(-1),
        password(password), clients(), pollset(), timeout_seconds(300), pong_timeout(15) {
    std::cout << "Server starting on port " << this->port
              << " with password '" << this->password << "'" << std::endl;
}

/* COPY CONSTRUCTOR */
Server::Server(const Server &other) { *this = other;}

/* OPERATORS */
Server &Server::operator=(const Server &other) {
    if (this != &other) {
        this->port = other.port;
        this->socket_fd = other.socket_fd;
        this->password = other.password;
        this->clients = other.clients;
        this->pollset = other.pollset;
        this->timeout_seconds = other.timeout_seconds;
        this->pong_timeout = other.pong_timeout;
    }
    return *this;
}

/* DESTRUCTOR */
Server::~Server(void) {}

/* SETTERS */
//void Server::setPortNumber(int other) {
//    this->port = other;
//}

//void Server::setServerPassword(std::string other) {
//    this->password = other;
//}

/* GETTERS */
int Server::getPortNumber(void) const {
    return this->port;
}

std::string Server::getServerPassword(void) const {
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
    if (this->socket_fd == -1)
        throwSystemError("socket");

    // Allow address reuse (prevents "Address already in use" error)
    int opt = 1;
    if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throwSystemError("setsockopt");

    // Setar O_NONBLOCK com fcntl()
    this->setNonBlocking(this->socket_fd);

    // bind socket to a IP/port
    std::cout << "Binding server socket to ip address" << std::endl;
    if (bind(this->socket_fd, (struct sockaddr *)&hint, sizeof(hint)) == -1 )
        throwSystemError("bind");

    // mark socket to start listening
    std::cout << "Marking socket to start listening" << std::endl;
    if (listen(this->socket_fd, SOMAXCONN) == -1)
        throwSystemError("listen");

    // CREATING POLL STRUCT FOR SERVER AND ADDING TO THE POLLFD STRUCT
    this->pollset.add(this->socket_fd);
}

/* MONITORING FOR ACTIVITY ON FDS */
void Server::monitorConnections(void) {
    // MONITORING FDS AND WAITING FOR EVENTS TO HAPPEN
    if (this->pollset.poll() == -1 && !Server::signals)
        throwSystemError("poll");

    std::cout << "poll waiting for an event to happen" << std::endl;
    // checking all fds
    for (size_t i = 0; i < pollset.getSize(); i++) {
        // CHECK IF THIS CURRENT SOCKET RECEIVED INPUT
        struct pollfd current = this->pollset.getPollFd(i);
        if (current.revents & POLLIN) {
            // CHECK IF ANY EVENTS HAPPENED ON SERVER SOCKET
			std::cout << "Client fd [" << current.fd << "] connected" << std::endl;
            if (current.fd == this->socket_fd) 
                this->connectClient(); // accept a new client
			else
                this->receiveData(i); // receive data for client that is already registered
        }
        else if (current.revents & POLLHUP || current.revents & POLLERR) {
            this->disconnectClient(i);
            --i;
        }
    }
    this->handleInactiveClients();
}

/* FIND FD INDEX IN POLLSET BY FD IN CLIENT */
size_t Server::getPollsetIdxByFd(int fd) {
    for (size_t i = 0; i < pollset.getPollfds().size(); i++) {
        if (pollset.getPollfds()[i].fd == fd)
            return i;
    }
    return -1;
}

/* SET SOCKETS AS NON BLOCKING */
void Server::setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        //("fcntl");
        if (socket != this->socket_fd)
            close(socket);
        throwSystemError("fcntl");
    }
    flags |= O_NONBLOCK;
    if (fcntl(socket, F_SETFL, flags) == -1) {
        if (socket != this->socket_fd)
            close(socket);
        throwSystemError("fcntl");
    }
}

/* ACCEPT A NEW CLIENT */
void Server::connectClient(void) {
    // accept a new client
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(this->socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) {
        int err = errno;
        std::cerr << "accept: " << strerror(err) << std::endl;
        return;
    }

    // Setar O_NONBLOCK with fcntl()
    this->setNonBlocking(client_fd);

    // CREATING POLL STRUCT FOR CURRENT CLIENT AND ADDING TO THE POLLFD STRUCT
    this->pollset.add(client_fd);

    Client client;
    client.setFd(client_fd);
    client.setLastActivity(std::time(0));
    this->clients.push_back(client);
    std::cout << "Last activity time: " << client.getLastActivity() << std::endl;
}

/* RECEIVE DATA FROM REGISTERED CLIENT */
void Server::receiveData(size_t &index) {
    char buffer[1024];
    struct pollfd current = this->pollset.getPollFd(index);
    ssize_t bytes_read = recv(current.fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read < 0) {
        int err = errno;
        std::cerr << "recv: " << strerror(err) << std::endl;
        this->disconnectClient(index);
        --index; // fix index after disconnecting
        return;
    }
    else if (bytes_read == 0) {
        std::cerr << "client disconnected" << std::endl;
        this->disconnectClient(index);
        --index; // fix index after disconnecting
        return;
    }
    else if (bytes_read > 0){
        buffer[bytes_read] = '\0';
        // print data received and stored in buffer
        Client *client = getClientByFd(current.fd);
        if (!client) {
            throw std::invalid_argument("Client fd not found");
        }
        if (client) {
            std::string buf = buffer;
            if (buf.empty() || buffer[0] == '\0')
                return; // ignora vazios
            this->handleClientMessage(*client, buf);
            client->appendData(buf);
            std::cout << "Client fd [" << client->getFd() << "]"
                  << " data: '" << client->getData() << "'" << std::endl;
            client->setLastActivity(std::time(0));
            client->setPingSent(false);
        }
    }
}

/* DISCONNECT CLIENT */
void Server::disconnectClient(size_t index) {
    // remove FD from pollset
    struct pollfd current = this->pollset.getPollFd(index);
    this->pollset.remove(index);

    // remove FD from clients vector
    std::vector<Client>::iterator it = clients.begin();
    for (size_t i = 0; i < clients.size(); i++) {
        if ((*it).getFd() == current.fd) {
            clients.erase(it);
            break;
        }
        it++;
    }
    // close FD
    close(current.fd);
}

/* FIND CLIENT BY FD */
Client *Server::getClientByFd(int fd_to_find) {
    std::vector<Client>::iterator it = this->clients.begin();
    for (size_t i = 0; i < clients.size(); i++) {
        if ((*it).getFd() == fd_to_find)
            return &clients[i];
        it++;
    }
    return NULL;
}

/* CLEAR RESOURCES */
void Server::clearServer(void) {
    // clear clients vector
    std::vector<Client>::iterator it = this->clients.begin();
    while (it != this->clients.end()) {
        close(it->getFd());
        it = this->clients.erase(it);
    }
    // close fds in struct pollfd
    this->pollset.clear();
}

/* THROW + SYSTEM ERROR MESSAGE */
void Server::throwSystemError(const std::string &msg){
    this->clearServer();
    int err = errno; // similar to perror(); returns the same error
    throw std::runtime_error(msg + ": " + strerror(err));
}


/* SIGNAL HANDLER FUNCTION */
void Server::signalHandler(int sig) {
    std::cout << "Program terminated with '" << sig << "'" << std::endl;
    (void)sig;
    Server::signals = true;
}

/* VERIFY CLIENTS ACTIVE TIME */
void Server::handleInactiveClients(void) {
    std::vector<Client>::iterator it = this->clients.begin();
    time_t now = std::time(0);

    while (it != this->clients.end()) {
        time_t lastActivity = it->getLastActivity();

        // if client inactive
        if (now - lastActivity >= this->timeout_seconds) {
            if (!it->pingSent()) { // if ping not sent
                std::stringstream ss; // convert time_t to string
                ss << now;
                std::string msg = "PING :" + ss.str() + "\r\n";
                send(it->getFd(), msg.c_str(), msg.length(), 0);

                it->setPingSent(true);
                it->setLastPingSent(now);

                std::cout << "Sent PING to Client with fd [" << it->getFd() << "]" << std::endl;
                ++it;
            }
            else if (now - it->getLastPingSent() >= this->pong_timeout) {
                // if ping was sent and timeout for pong is over
                std::cout << "Client with fd [" << it->getFd() << "] timeouted (no PONG received)" << std::endl;

                int poll_fd_idx = this->getPollsetIdxByFd(it->getFd());
                if (poll_fd_idx != -1) {
                    struct pollfd current = this->pollset.getPollFd(poll_fd_idx);
                    this->pollset.remove(poll_fd_idx);
                    close(current.fd);
                }
                it = this->clients.erase(it);
            }
            else { // if ping was sent an timeout for pong is still not over
                ++it;
            }
        } else { // if client is still active
            ++it;
        }
    }
}


/* HANDLER FOR MESSAGE */
/* only created now to deal with PONG */
void Server::handleClientMessage(Client &client, const std::string &msg) {
    // PONG: LALALA
    // get only the PONG and ignores the rest of the message
    std::istringstream iss(msg);
    std::string command;
    iss >> command;

    // check if pong was received
    if (command == "PONG") {
        if (std::time(0) - client.getLastPingSent() < this->pong_timeout) {
            client.setPingSent(false);
            client.setLastActivity(std::time(0));
            std::cout << "PONG received from Client with fd [" << client.getFd() << "]" << std::endl;
        }
        client.setLastActivity(std::time(0));
        return;
    }
}
