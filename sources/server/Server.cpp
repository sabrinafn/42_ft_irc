#include "../includes/server/Server.hpp"
#include "../includes/channel/channel.hpp"

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

std::map<std::string, Channel*> &Server::get_channels() {
  return channels;
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

    //std::cout << "poll waiting for an event to happen" << std::endl;
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
            std::cout << "Client fd [" << client->getFd() << "]"
                  << " buffer: '" << client->getData() << "'" << std::endl;
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
void Server::handleClientMessage(Client &client, const std::string &msg) {
    client.appendData(msg);
    
    std::string buffer = client.getData();
    std::vector<std::string> lines = Parser::extractLines(buffer);
    
    client.setData(buffer);
    
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
        std::cout << "Processing message from client [" << client.getFd() << "]: " << *it << std::endl;
        
        IRCMessage ircMsg = Parser::parseMessage(*it);
        if (!ircMsg.command.empty()) {
            processIRCMessage(client, ircMsg);
        }
    }
}

/* PROCESS IRC COMMANDS */
void Server::processIRCMessage(Client &client, const IRCMessage &msg) {
    if (msg.command == "PASS") {
        handlePass(client, msg);
    } else if (msg.command == "NICK") {
        handleNick(client, msg);
    } else if (msg.command == "USER") {
        handleUser(client, msg);
    } else if (msg.command == "PING") {
        handlePing(client, msg);
    } else if (msg.command == "PONG") {
        handlePong(client, msg);
    } else if (msg.command == "QUIT") {
        handleQuit(client, msg);
    } else if (msg.command == "JOIN"){
        handleJoin(client, msg);
    }  
    else {
        // Unknown command or not implemented yet
        if (client.getState() == REGISTERED) {
            sendReply(client.getFd(), 421, client.getNickname(), msg.command + " :Unknown command");
        }
    }
}

/* IRC COMMAND HANDLERS */
void Server::handlePass(Client &client, const IRCMessage &msg) {
    if (client.getState() != UNREGISTERED) {
        sendReply(client.getFd(), 462, client.getNickname(), ":You may not reregister");
        return;
    }
    
    if (msg.params.empty()) {
        sendReply(client.getFd(), 461, "*", "PASS :Not enough parameters");
        return;
    }
    
    if (msg.params[0] == this->password) {
        client.setState(PASS_OK);
        std::cout << "Client [" << client.getFd() << "] provided correct password" << std::endl;
    } else {
        sendReply(client.getFd(), 464, "*", ":Password incorrect");
        std::cout << "Client [" << client.getFd() << "] provided incorrect password" << std::endl;
    }
}

void Server::handleNick(Client &client, const IRCMessage &msg) {
    if (msg.params.empty()) {
        sendReply(client.getFd(), 431, "*", ":No nickname given");
        return;
    }
    
    std::string nickname = msg.params[0];
    
    if (!Parser::isValidNickname(nickname)) {
        sendReply(client.getFd(), 432, "*", nickname + " :Erroneous nickname");
        return;
    }
    
    if (isNicknameInUse(nickname, client.getFd())) {
        sendReply(client.getFd(), 433, "*", nickname + " :Nickname is already in use");
        return;
    }
    
    client.setNickname(nickname);
    std::cout << "Client [" << client.getFd() << "] set nickname to: " << nickname << std::endl;
    
    // Check if client is now fully registered
    if (client.getState() == PASS_OK && !client.getUsername().empty()) {
        client.setState(REGISTERED);
        sendWelcomeMessages(client);
    }
}

void Server::handleUser(Client &client, const IRCMessage &msg) {
    if (client.getState() == UNREGISTERED) {
        sendReply(client.getFd(), 464, "*", ":Password required");
        return;
    }
    
    if (!client.getUsername().empty()) {
        sendReply(client.getFd(), 462, client.getNickname(), ":You may not reregister");
        return;
    }
    
    if (msg.params.size() < 3 || msg.trailing.empty()) {
        sendReply(client.getFd(), 461, "*", "USER :Not enough parameters");
        return;
    }
    
    client.setUsername(msg.params[0]);
    client.setRealname(msg.trailing);
    std::cout << "Client [" << client.getFd() << "] set username: " << msg.params[0] 
              << ", realname: " << msg.trailing << std::endl;
    
    // Check if client is now fully registered
    if (client.getState() == PASS_OK && !client.getNickname().empty()) {
        client.setState(REGISTERED);
        sendWelcomeMessages(client);
    }
}

void Server::handlePing(Client &client, const IRCMessage &msg) {
    std::string server = msg.trailing.empty() ? 
        (msg.params.empty() ? "server" : msg.params[0]) : msg.trailing;
    sendRawMessage(client.getFd(), "PONG :" + server + "\r\n");
}

void Server::handlePong(Client &client, const IRCMessage &msg) {
    (void)msg; // Suppress unused parameter warning
    if (std::time(0) - client.getLastPingSent() < this->pong_timeout) {
        client.setPingSent(false);
        client.setLastActivity(std::time(0));
        std::cout << "PONG received from Client with fd [" << client.getFd() << "]" << std::endl;
    }
    client.setLastActivity(std::time(0));
}

void Server::handleQuit(Client &client, const IRCMessage &msg) {
    std::string quitMessage = msg.trailing.empty() ? "Client Quit" : msg.trailing;
    
    // Send quit message to client (optional)
    std::cout << "Client [" << client.getFd() << "] (" << client.getNickname() << ") quit: " << quitMessage << std::endl;
    
    // Find the client in pollset and disconnect
    size_t pollIndex = getPollsetIdxByFd(client.getFd());
    if (pollIndex != static_cast<size_t>(-1)) {
        disconnectClient(pollIndex);
    }
}

void Server::handleJoin(Client &client, const IRCMessage &msg)
{
        Channel *Channel;
        if (msg.params.empty()) {
        sendReply(client.getFd(), 461, "*", "JOIN :Not enough parameters");
        return;
        }
                //if(nao esta autenticado)
        std::vector<std::string> Channels;
	    std::vector<std::string> keys;
	    std::stringstream ss(msg.params[0]);
	    std::string channelName;
        while (std::getline(ss, channelName, ',')) {
            if(!isValidChannelName(channelName))
                return;
            if (!channelName.empty())
                Channels.push_back(channelName);
        }
        if (msg.params.size() > 1) {
		std::stringstream ss_keys(msg.params[1]);
		std::string key;
		while (std::getline(ss_keys, key, ',')) {
            if(!isValidkey(key))
                return;
			keys.push_back(key);
		}
        for (size_t i = 0; i < Channels.size(); ++i) {
            const std::string& channelName = Channels[i];

            if (channels.find(channelName) == channels.end())
            {	
                channels[channelName] = new Channel(channelName);
                channels[channelName]->addOperator(clients);
            }
            else
            {
                if (channel->getMembers().find(sender.get_fd()) != channel->getMembers().end()) {
				server.send_message(sender.get_fd(), ERR_USERONCHANNEL(sender.get_nickname(), channel_name));
				return ;
			}
			if (channel->mode('l') && channel->getCurrentMembersCount() >= channel->getUserLimit()) {
				server.send_message(sender.get_fd(), ERR_CHANNELISFULL(channel_name));
				return ;
			}
			if (channel->mode('i') && !channel->isInvited(&sender)) {
				server.send_message(sender.get_fd(), ERR_INVITEONLYCHAN(channel_name));
				return ;
			}
			if (channel->mode('k') && i < keys.size()) {
				if (channel->getKey() != keys[i]) {
					server.send_message(sender.get_fd(), ERR_BADCHANNELKEY(sender.get_username(), channel_name));
					return ;
				} 
            }
        }


	}

        //if(canal existe,entrar no canal){
        //    verificar dentro desse if os modos +i +k +l }
        // if(cliente ja esta no canal) ignorar ou retornar erro
        //
        //else( se nao existe criar o canal)
        //
        //
        //retornar topcp do canal ou retornar que nao tem topic
        //retornar lista de usuarios e fim da lista 
    }

/* SEND IRC REPLY TO CLIENT */
void Server::sendReply(int fd, int code, const std::string& nickname, const std::string& message) {
    std::stringstream ss;
    ss << ":localhost " << code << " " << nickname << " " << message << "\r\n";
    std::string reply = ss.str();
    send(fd, reply.c_str(), reply.length(), 0);
    std::cout << "Sent to client [" << fd << "]: " << reply.substr(0, reply.length() - 2) << std::endl;
}

void Server::sendRawMessage(int fd, const std::string& message) {
    send(fd, message.c_str(), message.length(), 0);
    std::cout << "Sent to client [" << fd << "]: " << message.substr(0, message.length() - 2) << std::endl;
}

/* CHECK IF NICKNAME IS ALREADY IN USE */
bool Server::isNicknameInUse(const std::string& nickname, int excludeFd) {
    std::vector<Client>::iterator it = this->clients.begin();
    for (; it != this->clients.end(); ++it) {
        if (it->getFd() != excludeFd && it->getNickname() == nickname) {
            return true;
        }
    }
    return false;
}

void Server::sendWelcomeMessages(Client &client) {
    std::string nick = client.getNickname();
    sendReply(client.getFd(), 001, nick, ":Welcome to the Internet Relay Network " + nick);
    sendReply(client.getFd(), 002, nick, ":Your host is localhost, running version 1.0");
    sendReply(client.getFd(), 003, nick, ":This server was created today");
    sendReply(client.getFd(), 004, nick, "localhost 1.0 o o");
    std::cout << "Client [" << client.getFd() << "] (" << nick << ") is now registered!" << std::endl;
}
