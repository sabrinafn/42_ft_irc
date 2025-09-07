#include "../includes/ft_irc.hpp"

bool Server::signals = false;

/* CONSTRUCTORS */
Server::Server(void)
    : port(-1),
      socket_fd(-1),
      password(""),
      pollset(),
      clients(),
      timeout_seconds(300),
      pong_timeout(20),
      max_clients(100) {
}

Server::Server(int port, const std::string &password)
    : port(port),
      socket_fd(-1),
      password(password),
      pollset(),
      clients(),
      channels(),
      timeout_seconds(300),
      pong_timeout(20),
      max_clients(100) {
    std::stringstream ss;
    ss << "Server starting on port " << this->port << " with password '" << this->password
       << "' (max clients: " << this->max_clients << ")";
    logInfo(ss.str());
}

/* COPY CONSTRUCTOR */
Server::Server(const Server &other) {
    *this = other;
}

/* OPERATORS */
Server &Server::operator=(const Server &other) {
    if (this != &other) {
        this->port            = other.port;
        this->socket_fd       = other.socket_fd;
        this->password        = other.password;
        this->pollset         = other.pollset;
        this->clients         = other.clients;
        this->channels        = other.channels;
        this->signals         = other.signals;
        this->timeout_seconds = other.timeout_seconds;
        this->pong_timeout    = other.pong_timeout;
        this->max_clients     = other.max_clients;
    }
    return *this;
}

/* DESTRUCTOR */
Server::~Server(void) {
    for (size_t i = 0; i < clients.size(); ++i) {
        close(clients[i]->getFd());
        delete clients[i];
    }
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        delete it->second;
    }
    channels.clear();
    clients.clear();
    this->pollset.clear();
    if (this->socket_fd != -1){
        close(this->socket_fd);
        this->socket_fd = -1;
    }
}

/* GETTERS */
int Server::getPortNumber(void) const {
    return this->port;
}

std::string Server::getServerPassword(void) const {
    return this->password;
}

const std::vector<Client *> &Server::getClients() const {
    return clients;
}

std::map<std::string, Channel *> &Server::get_channels() {
    return channels;
}

int Server::getPongTimeout(void) const {
    return this->pong_timeout;
}

int Server::getMaxClients(void) const {
    return this->max_clients;
}

Client *Server::getClientByNick(const std::string &nick) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i]->getNickname() == nick)
        return clients[i];
    }
    return NULL;
}

size_t Server::getPollsetIdxByFd(int fd) {
    for (size_t i = 0; i < pollset.getPollfds().size(); i++) {
        if (pollset.getPollfds()[i].fd == fd)
        return i;
    }
    return -1;
}

Client *Server::getClientByFd(int fd_to_find) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i]->getFd() == fd_to_find)
        return clients[i];
    }
    return NULL;
}

/* INIT SERVER */
void Server::initServer(void) {
    createSocket();
    while (!Server::signals) {
        monitorConnections();
    }
}

/* CREATE SOCKET */
void Server::createSocket(void) {
    sockaddr_in hint;
    hint.sin_family      = AF_INET;           // IPV4
    hint.sin_port        = htons(this->port); // convert number to network byte order
    hint.sin_addr.s_addr = INADDR_ANY;        // set the address to any local machine address
    
    // create socket
    logInfo("Creating server socket...");
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
    logInfo("Binding server socket to ip address");
    if (bind(this->socket_fd, (struct sockaddr *)&hint, sizeof(hint)) == -1)
    throwSystemError("bind");
    
    // mark socket to start listening
    logInfo("Marking socket to start listening");
    if (listen(this->socket_fd, SOMAXCONN) == -1)
    throwSystemError("listen");
    
    // CREATING POLL STRUCT FOR SERVER AND ADDING TO THE POLLFD STRUCT
    this->pollset.add(this->socket_fd);
}

/* MONITORING FOR ACTIVITY ON FDS */
void Server::monitorConnections(void) {
    if (this->pollset.poll() == -1 && !Server::signals)
    throwSystemError("poll");
    
    for (size_t i = 0; i < pollset.getSize(); i++) {
        struct pollfd current = this->pollset.getPollFd(i);
        if (current.revents & POLLIN) {
            if (current.fd == this->socket_fd) {
                std::stringstream ss;
                ss << "Server listening socket [" << current.fd
                << "] is ready to accept a new client";
                logInfo(ss.str());
                this->connectClient(); // accept a new client
            } else {
                std::stringstream ss;
                ss << "Client [" << current.fd << "] is sending new data";
                logDebug(ss.str());
                this->receiveData(i); // receive data
            }
        } else if (current.revents & POLLHUP || current.revents & POLLERR) {
            this->disconnectClient(i);
            --i;
        }
    }
    this->handleInactiveClients();
}

/* SET SOCKETS AS NON BLOCKING */
void Server::setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
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

    if (this->clients.size() >= static_cast<size_t>(this->max_clients)) {
        logInfo("Server is full, rejecting new connection");
        return;
    }
    
    struct sockaddr_in client_addr;
    socklen_t          client_addr_len = sizeof(client_addr);
    int client_fd = accept(this->socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) {
        int err = errno;
        logError(std::string("accept: ") + strerror(err));
        return;
    }
    
    // Double-check connection limit after accept (race condition protection)
    if (this->clients.size() >= static_cast<size_t>(this->max_clients)) {
        logInfo("Server is full, closing new connection");
        close(client_fd);
        return;
    }
    
    this->setNonBlocking(client_fd);
    this->pollset.add(client_fd);
    
    Client *client = new Client();
    client->setFd(client_fd);
    client->setLastActivity(std::time(0));
    client->setIpAddress(std::string(inet_ntoa(client_addr.sin_addr)));
    this->clients.push_back(client);
    
    std::stringstream ss;
    ss << "New client connected [" << client_fd << "] from " << client->getIpAddress() 
       << " (clients: " << this->clients.size() << "/" << this->max_clients << ")";
    logInfo(ss.str());
}

/* RECEIVE DATA FROM REGISTERED CLIENT */
void Server::receiveData(size_t &index) {
    char          buffer[1024];
    struct pollfd current    = this->pollset.getPollFd(index);
    ssize_t       bytes_read = recv(current.fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read < 0) {
        int err = errno;
        logError(std::string("recv: ") + strerror(err));
        this->disconnectClient(index);
        --index;
        return;
    } else if (bytes_read == 0) {
        logInfo("client disconnected");
        this->disconnectClient(index);
        --index;
        return;
    } else if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        Client *client     = getClientByFd(current.fd);
        if (!client) {
            throw std::invalid_argument("Client fd not found");
        }
        std::string buf = buffer;
        if (buf.empty() || buffer[0] == '\0')
        return;
        this->handleClientMessage(*client, buf);
        
        // Check if client still exists after handling message (might have been deleted by QUIT)
        client = getClientByFd(current.fd);
        if (client) {
            client->setLastActivity(std::time(0));
            client->setPingSent(false);
        }
    }
}

/* DISCONNECT CLIENT */
void Server::disconnectClient(size_t index) {
    struct pollfd current = this->pollset.getPollFd(index);
    this->pollset.remove(index);
    std::vector<Client *>::iterator it = clients.begin();
    for (size_t i = 0; i < clients.size(); i++) {
        if ((*it)->getFd() == current.fd) {
            delete *it;
            clients.erase(it);
            break;
        }
        it++;
    }
    close(current.fd);
}


/* THROW + SYSTEM ERROR MESSAGE */
void Server::throwSystemError(const std::string &msg) {
    int err = errno; // similar to perror(); returns the same error
    throw std::runtime_error(msg + ": " + strerror(err));
}

/* SIGNAL HANDLER FUNCTION */
void Server::signalHandler(int sig) {
    std::stringstream ss;
    ss << "Program terminated with '" << sig << "'";
    logInfo(ss.str());
    (void)sig;
    Server::signals = true;
}

/* VERIFY CLIENTS ACTIVE TIME */
void Server::handleInactiveClients(void) {
    std::vector<Client *>::iterator it  = this->clients.begin();
    time_t                          now = std::time(0);
    
    while (it != this->clients.end()) {
        Client* client = *it;
        
        if (isClientTimedOut(client, now)) {
            if (!client->pingSent()) {
                sendPing(client, now);
                ++it;
            } else if (isPongTimeout(client, now)) {
                removeTimedOutClient(client);
                it = this->clients.erase(it);
            }
            else { // ping sent and pong timeout is not over
                ++it;
            }
        }
        else { // if client is still active
            ++it;
        }
    }
}

bool Server::isClientTimedOut(Client* client, time_t now) {
    return (now - client->getLastActivity() >= timeout_seconds);
}

bool Server::isPongTimeout(Client* client, time_t now) {
    return (now - client->getLastPingSent() >= pong_timeout);
}

void Server::sendPing(Client* client, time_t now) {
    std::stringstream ss;
    ss << now;
    std::string msg = "PING :" + ss.str() + "\r\n";
    send(client->getFd(), msg.c_str(), msg.length(), 0);
    client->setPingSent(true);
    client->setLastPingSent(now);
    std::stringstream ss2;
    ss2 << "Sent PING to Client with fd [" << client->getFd() << "]";
    logDebug(ss2.str());
}

void Server::removeTimedOutClient(Client *client) {
    std::stringstream ss;
    ss << "Client with fd [" << client->getFd() << "] timeouted (no PONG received)";
    logInfo(ss.str());
    
    int poll_fd_idx = this->getPollsetIdxByFd(client->getFd());
    if (poll_fd_idx != -1) {
        struct pollfd current = this->pollset.getPollFd(poll_fd_idx);
        this->pollset.remove(poll_fd_idx);
        close(current.fd);
    }
    delete client;
}

/* HANDLER FOR MESSAGE */
void Server::handleClientMessage(Client &client, const std::string &msg) {
    client.appendData(msg);
    Commands commands;
    
    std::string              buffer = client.getData();
    std::vector<std::string> lines  = Parser::extractLines(buffer);
    
    client.setData(buffer);
    
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
        std::stringstream ss;
        ss << "Processing message from client [" << client.getFd() << "]: " << *it;
        logDebug(ss.str());
        IRCMessage ircMsg = Parser::parseMessage(*it);
        if (!ircMsg.command.empty()) {
            commands.handler(client, *this, ircMsg);
        }
    }
}

/* CHANNEL HANDLER METHODS */
void Server::setChannel(Channel *new_channel) {
    channels[new_channel->getName()] = new_channel;
}

bool Server::hasChannel(const std::string &channel_name) {
    if (channels.find(channel_name) == channels.end())
    return false;
    return true;
}

void Server::removeChannel(const std::string &channel_name) {
    std::map<std::string, Channel *>::iterator it = channels.find(channel_name);
    if (it != channels.end()) {
        delete it->second;
        channels.erase(it);
    }
}
