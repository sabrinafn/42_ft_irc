#include "../includes/server/Server.hpp"
#include "../includes/channel/Channel.hpp"

bool Server::signals = false;

/* CONSTRUCTOR */
// construtores
Server::Server(void) : port(-1), socket_fd(-1), password(""),
    clients(), pollset(), timeout_seconds(300), pong_timeout(20) {}

Server::Server(int port, const std::string &password) : port(port), socket_fd(-1),
        password(password), clients(), pollset(), timeout_seconds(300), pong_timeout(20) {
    std::cout << "Server starting on port " << this->port
              << " with password '" << this->password << "'" << std::endl;
}

/* COPY CONSTRUCTOR */
Server::Server(const Server &other) { *this = other; }

/* OPERATORS */
Server &Server::operator=(const Server &other) {
    if (this != &other) {
        this->port = other.port;
        this->socket_fd = other.socket_fd;
        this->password = other.password;
        // copia apenas ponteiros, não objetos
        this->clients = other.clients;
        this->pollset = other.pollset;
        this->timeout_seconds = other.timeout_seconds;
        this->pong_timeout = other.pong_timeout;
    }
    return *this;
}

// destrutor
Server::~Server(void) {
    // libera memoria dos clients
    for (size_t i = 0; i < clients.size(); ++i) {
        delete clients[i];
    }
    clients.clear();
}

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
    this->setNonBlocking(client_fd);
    this->pollset.add(client_fd);

    Client* client = new Client();
    client->setFd(client_fd);
    client->setLastActivity(std::time(0));
    this->clients.push_back(client);
    std::cout << "Last activity time: " << client->getLastActivity() << std::endl;
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
        --index;
        return;
    }
    else if (bytes_read == 0) {
        std::cerr << "client disconnected" << std::endl;
        this->disconnectClient(index);
        --index;
        return;
    }
    else if (bytes_read > 0){
        buffer[bytes_read] = '\0';
        Client *client = getClientByFd(current.fd);
        if (!client) {
            throw std::invalid_argument("Client fd not found");
        }
        std::string buf = buffer;
        if (buf.empty() || buffer[0] == '\0')
            return;
        this->handleClientMessage(*client, buf);
        std::cout << "Client fd [" << client->getFd() << "]"
              << " buffer: '" << client->getData() << "'" << std::endl;
        client->setLastActivity(std::time(0));
        client->setPingSent(false);
    }
}

/* DISCONNECT CLIENT */
void Server::disconnectClient(size_t index) {
    struct pollfd current = this->pollset.getPollFd(index);
    this->pollset.remove(index);

    // remove e deleta o ponteiro do client
    std::vector<Client*>::iterator it = clients.begin();
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

/* FIND CLIENT BY FD */
Client *Server::getClientByFd(int fd_to_find) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i]->getFd() == fd_to_find)
            return clients[i];
    }
    return NULL;
}

/* CLEAR RESOURCES */
void Server::clearServer(void) {
    for (size_t i = 0; i < clients.size(); ++i) {
        close(clients[i]->getFd());
        delete clients[i];
    }
    clients.clear();
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
    std::vector<Client*>::iterator it = this->clients.begin();
    time_t now = std::time(0);

    while (it != this->clients.end()) {
        time_t lastActivity = (*it)->getLastActivity();

        // if client inactive
        if (now - lastActivity >= this->timeout_seconds) {
            if (!(*it)->pingSent()) { // if ping not sent
                std::stringstream ss; // convert time_t to string
                ss << now;
                std::string msg = "PING :" + ss.str() + "\r\n";
                send((*it)->getFd(), msg.c_str(), msg.length(), 0);

                (*it)->setPingSent(true);
                (*it)->setLastPingSent(now);

                std::cout << "Sent PING to Client with fd [" << (*it)->getFd() << "]" << std::endl;
                ++it;
            }
            else if (now - (*it)->getLastPingSent() >= this->pong_timeout) {
                // if ping was sent and timeout for pong is over
                std::cout << "Client with fd [" << (*it)->getFd() << "] timeouted (no PONG received)" << std::endl;

                int poll_fd_idx = this->getPollsetIdxByFd((*it)->getFd());
                if (poll_fd_idx != -1) {
                    struct pollfd current = this->pollset.getPollFd(poll_fd_idx);
                    this->pollset.remove(poll_fd_idx);
                    close(current.fd);
                }
                delete *it;
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
    } else if (msg.command == "PRIVMSG"){
        handlePrivmsg(client, msg);}
     else if (msg.command == "TOPIC"){
         handleTopic(client, msg);
     }
    else {
        // Unknown command or not implemented yet
        if (client.getState() == REGISTERED) {
            client.sendReply(ERR_UNKNOWNCMD(msg.command));
        }
    }
}


/* IRC COMMAND HANDLERS */
void Server::handlePass(Client &client, const IRCMessage &msg) {
    if (client.getState() != UNREGISTERED) {
        client.sendReply(ERR_ALREADYREGISTRED(client.getNickname()));
        return;
    }
    
    if (msg.params.empty()) {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }
    
    if (msg.params[0] == this->password) {
        client.setState(PASS_OK);
        std::cout << "Client [" << client.getFd() << "] provided correct password" << std::endl;
    } else {
        client.sendReply(ERR_PASSWDMISMATCH());
        std::cout << "Client [" << client.getFd() << "] provided incorrect password" << std::endl;
    }
}

void Server::handleNick(Client &client, const IRCMessage &msg) {
    if (msg.params.empty()) {
        client.sendReply(ERR_NONICKNAMEGIVEN());
        return;
    }
    
    std::string nickname = msg.params[0];
    
    if (!Parser::isValidNickname(nickname)) {
        client.sendReply(ERR_ERRONEUSNICKNAME(nickname));
        return;
    }
    
    if (isNicknameInUse(nickname, client.getFd())) {
        client.sendReply(ERR_NICKNAMEINUSE(nickname));
        return;
    }
    
    client.setNickname(nickname);
    std::cout << "Client [" << client.getFd() << "] set nickname to: " << nickname << std::endl;
    
    // Check if client is now fully registered
    if (client.getState() == PASS_OK && !client.getUsername().empty()) {
        client.setState(REGISTERED);
        client.sendWelcomeMessages();
        //sendWelcomeMessages(client);
    }
}

void Server::handleUser(Client &client, const IRCMessage &msg) {
    if (client.getState() == UNREGISTERED) {
        client.sendReply(ERR_PASSWDMISMATCH());
        return;
    }
    
    if (!client.getUsername().empty()) {
        client.sendReply(ERR_ALREADYREGISTRED(client.getNickname()));
        return;
    }
    
    if (msg.params.size() < 3 || msg.trailing.empty()) {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }
    
    client.setUsername(msg.params[0]);
    client.setRealname(msg.trailing);
    std::cout << "Client [" << client.getFd() << "] set username: " << msg.params[0] 
              << ", realname: " << msg.trailing << std::endl;
    
    // Check if client is now fully registered
    if (client.getState() == PASS_OK && !client.getNickname().empty()) {
        client.setState(REGISTERED);
        //sendWelcomeMessages(client);
        client.sendWelcomeMessages();
    }
}

void Server::handlePing(Client &client, const IRCMessage &msg) {
    std::string server = msg.trailing.empty() ? (msg.params.empty() ? "server" : msg.params[0]) : msg.trailing;
    client.sendReply("PONG :" + server + "\r\n");
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
    std::cout << "DEBUG: handleJoin chamado para cliente " << client.getNickname() << std::endl;

    // verifica se tem parametros depois do JOIN
    if (msg.params.empty()) {
        std::cout << "DEBUG: JOIN sem parametros" << std::endl;
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }

    // faz o parse dos nomes dos canais e dos modes
    std::vector<std::string> channelNames;
    std::vector<std::string> modes;
    std::stringstream ss(msg.params[0]);
    std::string channelName;
    while (std::getline(ss, channelName, ',')) {
        std::cout << "DEBUG: verificando nome de canal: " << channelName << std::endl;
        if (!isValidChannelName(channelName)) {
            std::cout << "DEBUG: nome de canal invalido: " << channelName << std::endl;
            return;
        }
        if (!channelName.empty())
            channelNames.push_back(channelName);
    }

    // faz o parse das keys se existirem
    if (msg.params.size() > 1) {
        std::stringstream ss_keys(msg.params[1]);
        std::string key;
        while (std::getline(ss_keys, key, ',')) {
            std::cout << "DEBUG: verificando key do canal: " << key << std::endl;
            if (!isValidkey(key)) {
                std::cout << "DEBUG: key invalida: " << key << std::endl;
                return;
            }
            modes.push_back(key);
        }
    }

    std::cout << "DEBUG: total de canais para JOIN: " << channelNames.size() << std::endl;

    // loop em todos os canais
    for (size_t i = 0; i < channelNames.size(); ++i) {
        const std::string& name = channelNames[i];
        Channel *channel = NULL;

        std::cout << "DEBUG: processando canal " << name << std::endl;

        // cria o canal se ele nao existir
        if (channels.find(name) == channels.end()) {
            std::cout << "DEBUG: criando canal " << name << std::endl;
            channel = new Channel(name);
            channels[name] = channel;
            channel->addOperator(&client);
        } else {
            channel = channels[name];
            std::cout << "DEBUG: canal ja existe " << name << std::endl;
        }

        // verifica se o usuario ja eh membro desse canal
        if (channel->isMember(&client)) {
            std::cout << "DEBUG: cliente ja eh membro do canal " << name << std::endl;
            client.sendReply(ERR_USERONCHANNEL(client.getNickname(), channel->getName()));
            continue;
        }

        // verifica limite de usuarios (+l)
        if (channel->hasMode(Channel::LIMIT_SET) && (int)channel->getMembers().size() >= channel->getLimit()) {
            std::cout << "DEBUG: canal " << name << " atingiu limite de usuarios" << std::endl;
            client.sendReply(ERR_CHANNELISFULL(channel->getName()));
            continue;
        }

        // verifica se o canal eh invite only (+i)
        if (channel->hasMode(Channel::INVITE_ONLY) && !channel->isInvited(&client)) {
            std::cout << "DEBUG: cliente nao convidado para canal " << name << std::endl;
            client.sendReply(ERR_INVITEONLYCHAN(channel->getName()));
            continue;
        }

        // verifica se precisa de senha (+k)
        if (channel->hasMode(Channel::KEY_REQUIRED)) {
            if (i >= modes.size() || channel->getKey() != modes[i]) {
                std::cout << "DEBUG: key incorreta para canal " << name << std::endl;
               client.sendReply(ERR_BADCHANNELKEY(client.getUsername(), channel->getName()));
                continue;
            }
        }

        // adiciona o usuario como membro do canal
        channel->addMember(&client);
        std::cout << "DEBUG: cliente adicionado ao canal " << name << std::endl;

        // broadcast do JOIN para todos membros
        std::string joinMsg = ":" + client.getPrefix() + " JOIN :" + name;
        std::cout << "DEBUG: broadcast JOIN -> " << joinMsg << std::endl;
        channel->broadcast(joinMsg);

        // remove o convite desse usuario da lista de convidados
        if (channel->isInvited(&client)) {
            std::cout << "DEBUG: removendo convite do cliente " << client.getNickname() << std::endl;
            channel->removeInvite(&client);
        }

        // envia informacoes do topico do canal
        std::string topic = channel->getTopic();
        if (topic.empty()) {
            std::cout << "DEBUG: canal " << name << " nao tem topico" << std::endl;
            client.sendReply(RPL_NOTOPIC(client.getNickname(), channel->getName()));
        } else {
            std::cout << "DEBUG: topico do canal " << name << " -> " << topic << std::endl;
            client.sendReply(RPL_TOPIC(client.getNickname(), channel->getName(), channel->getTopic()));
        }

        // monta a lista de usuarios do canal
        std::string namesReply = "=";
        std::vector<Client*> members = channel->getMembers();
        std::cout << "DEBUG: membros do canal " << name << ":";
        for (size_t j = 0; j < members.size(); ++j) {
            if (channel->isOperator(members[j])) {
                namesReply += " @" + members[j]->getNickname();
                std::cout << " @" << members[j]->getNickname();
            } else {
                namesReply += " " + members[j]->getNickname();
                std::cout << " " << members[j]->getNickname();
            }
        }
        std::cout << std::endl;

        // envia a lista de usuarios
        client.sendReply(RPL_NAMREPLY(client.getNickname(), channel->getName(), namesReply));
        // envia fim da lista de usuarios
        client.sendReply(RPL_ENDOFNAMES(client.getNickname(), channel->getName()));

        std::cout << "DEBUG: handleJoin finalizado para canal " << name << std::endl;
    }
}

/* CHECK IF NICKNAME IS ALREADY IN USE */
bool Server::isNicknameInUse(const std::string& nickname, int excludeFd) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i]->getFd() != excludeFd && clients[i]->getNickname() == nickname) {
            return true;
        }
    }
    return false;
}

