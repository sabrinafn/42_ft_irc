#include "../includes/commands/Commands.hpp"

/* CONSTRUCTOR */
Commands::Commands(void) {
    commandsMap["PASS"]    = &Commands::handlePass;
    commandsMap["NICK"]    = &Commands::handleNick;
    commandsMap["USER"]    = &Commands::handleUser;
    commandsMap["PING"]    = &Commands::handlePing;
    commandsMap["PONG"]    = &Commands::handlePong;
    commandsMap["QUIT"]    = &Commands::handleQuit;
    commandsMap["JOIN"]    = &Commands::handleJoin;
    // commandsMap["PRIVMSG"] = &Commands::handlePrivmsg;
    // commandsMap["TOPIC"]   = &Commands::handleTopic;
    // commandsMap["KICK"]    = &Commands::handleKick;
    // commandsMap["INVITE"]  = &Commands::handleInvite;
    // commandsMap["MODE"]    = &Commands::handleMode;
}

/* COPY CONSTRUCTOR */
Commands::Commands(const Commands& other) {
    *this = other;
}

/* = OPERATOR */
Commands& Commands::operator=(const Commands& other) {
    (void)other;
    return *this;
}

/* DESTRUCTOR */
Commands::~Commands(void) {}

/* COMMAND HANDLER THAT WILL CALL EACH COMMAND */
void Commands::handler(Client &client, Server &server, const IRCMessage &msg) {
    std::map<std::string, CommandFunc>::iterator it = this->commandsMap.find(msg.command);
    std::cout << "DEBUG: Commands::handler called" << std::endl;
    if (it != commandsMap.end()) {
        CommandFunc func = it->second;
        (this->*func)(client, server, msg);
    } else {
        if (client.getState() == REGISTERED) {
            client.sendReply(ERR_UNKNOWNCMD(msg.command));
        }
    }
}

/*************************************************************/

/* HANDLEPASS */
void Commands::handlePass(Client &client, Server &server, const IRCMessage &msg) {
    if (client.getState() != UNREGISTERED) {
        client.sendReply(ERR_ALREADYREGISTRED(client.getNickname()));
        return;
    }    
    if (msg.params.empty()) {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }
    if (msg.params[0] == server.getServerPassword()) {
        client.setState(PASS_OK);
        std::cout << "DEBUG: Client " << client.getFd() << " provided correct password" << std::endl;
    } else {
        client.sendReply(ERR_PASSWDMISMATCH());
        std::cout << "ERROR: Client " << client.getFd() << " provided an incorrect password" << std::endl;
    }
}

/* HANDLENICK */
void Commands::handleNick(Client &client, Server &server, const IRCMessage &msg) {
    if (msg.params.empty()) {
        client.sendReply(ERR_NONICKNAMEGIVEN());
        return;
    }
    
    std::string nickname = msg.params[0];
    
    if (!Parser::isValidNickname(nickname)) {
        client.sendReply(ERR_ERRONEUSNICKNAME(nickname));
        return;
    }
    
    if (isNicknameInUse(server, nickname, client.getFd())) {
        client.sendReply(ERR_NICKNAMEINUSE(nickname));
        return;
    }
    
    client.setNickname(nickname);
    std::cout << "INFO: Client [" << client.getFd() << "] set nickname to: " << nickname << std::endl;
    
    // Check if client is now fully registered
    if (client.getState() == PASS_OK && !client.getUsername().empty()) {
        client.setState(REGISTERED);
        client.sendWelcomeMessages();
    }
}

/* CHECK IF NICKNAME IS ALREADY IN USE */
bool Commands::isNicknameInUse(Server &server, const std::string& nickname, int excludeFd) {
    const std::vector<Client*>& clients = server.getClients();
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i]->getFd() != excludeFd && clients[i]->getNickname() == nickname) {
            return true;
        }
    }
    return false;
}

/* HANDLEUSER */
void Commands::handleUser(Client &client, Server &server, const IRCMessage &msg) {
    (void)server;
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
    std::cout << "INFO: Client [" << client.getFd() << "] set username: " << msg.params[0] 
              << ", realname: " << msg.trailing << std::endl;
    
    // Check if client is now fully registered
    if (client.getState() == PASS_OK && !client.getNickname().empty()) {
        client.setState(REGISTERED);
        client.sendWelcomeMessages();
    }
}

void Commands::handlePing(Client &client, Server &server, const IRCMessage &msg) {
    (void)server;
    std::string servername = msg.trailing.empty() ? (msg.params.empty() ? "server" : msg.params[0]) : msg.trailing;
    client.sendReply("PONG :" + servername + "\r\n");
}

void Commands::handlePong(Client &client, Server &server, const IRCMessage &msg) {
    (void)msg; // Suppress unused parameter warning
    if (std::time(0) - client.getLastPingSent() < server.getPongTimeout()) {
        client.setPingSent(false);
        client.setLastActivity(std::time(0));
        std::cout << "PONG received from Client with fd [" << client.getFd() << "]" << std::endl;
    }
    client.setLastActivity(std::time(0));
}

/* HANDLEQUIT */
void Commands::handleQuit(Client &client, Server &server, const IRCMessage &msg) {
    std::string quitMessage = msg.trailing.empty() ? "Client Quit" : msg.trailing;
    
    // Send quit message to client (optional)
    std::cout << "Client [" << client.getFd() << "] (" << client.getNickname() << ") quit: " << quitMessage << std::endl;
    
    // Find the client in pollset and disconnect
    size_t pollIndex = server.getPollsetIdxByFd(client.getFd());
    if (pollIndex != static_cast<size_t>(-1)) {
        server.disconnectClient(pollIndex);
    }
}

/* HANDLEJOIN */

void Commands::handleJoin(Client &client, Server &server, const IRCMessage &msg)
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
        if (server.get_channels().find(name) == server.get_channels().end()) {
            std::cout << "DEBUG: criando canal " << name << std::endl;
            channel = new Channel(name);
            //channels[name] = channel;
            server.setChannel(channel);
            channel->addOperator(&client);
        } else {
            //channel = channels[name];
            std::cout << "ERROR: canal ja existe " << name << std::endl;
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

bool Commands::isValidChannelName(const std::string& name) {

    if (name[0] != '#' && name[0] != '&')
        return false;

    for (size_t i = 1; i < name.size(); ++i) {
        unsigned char c = name[i];
        if (std::isspace(c))         // espaço
            return false;
        if (c == ':')                // caractere especial proibido
            return false;
        if (std::iscntrl(c))         // caracteres de controle
            return false;
    }
	return true;
}

bool Commands::isValidkey(std::string key){

	   for (size_t i = 1; i < key.size(); ++i) {
        unsigned char c = key[i];
        if (std::isspace(c))         // espaço
            return false;
        if (c == ':')                // caractere especial proibido
            return false;
        if (std::iscntrl(c))         // caracteres de controle
            return false;
	   }
       return true;
}