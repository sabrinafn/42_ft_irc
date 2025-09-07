#include "../includes/ft_irc.hpp"

/* HANDLEJOIN */

void Commands::handleJoin(Client &client, Server &server, const IRCMessage &msg) {
    // verifica se tem parametros depois do JOIN
    if (msg.params.empty()) {
        logError("JOIN has no arguments");
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }

    // faz o parse dos nomes dos canais e dos modes
    std::vector<std::string> channelNames;
    std::vector<std::string> modes;
    std::stringstream        ss(msg.params[0]);
    std::string              channelName;
    while (std::getline(ss, channelName, ',')) {
        std::stringstream ss;
        ss << "check channel name: " << channelName;
        logDebug(ss.str());
        if (!isValidChannelName(channelName)) {
            std::stringstream ss2;
            ss2 << "Invalid channel name: " << channelName;
            logError(ss2.str());
            client.sendReply(ERR_NOSUCHCHANNEL(channelName));
            continue;
        }
        if (!channelName.empty())
            channelNames.push_back(channelName);
    }

    // faz o parse das keys se existirem
    if (msg.params.size() > 1) {
        std::stringstream ss_keys(msg.params[1]);
        std::string       key;
        while (std::getline(ss_keys, key, ',')) {
            std::stringstream ss3;
            ss3 << "check channel key password: " << key;
            logDebug(ss3.str());
            if (!isValidkey(key)) {
                std::stringstream ss4;
                ss4 << "Invalid key password: " << key;
                logError(ss4.str());
                return;
            }
            modes.push_back(key);
        }
    }

    std::stringstream strs;
    strs << "Number of channels to JOIN: " << channelNames.size();
    logDebug(strs.str());

    // loop em todos os canais
    for (size_t i = 0; i < channelNames.size(); ++i) {
        const std::string &name    = channelNames[i];
        Channel           *channel = NULL;

        logDebug("Processing channel " + name);

        // cria o canal se ele nao existir
        if (server.get_channels().find(name) == server.get_channels().end()) {
            logDebug("Creating channel " + name);
            channel = new Channel(name);
            server.setChannel(channel);
            // adiciona o usuario como membro do canal
            channel->addMember(&client);
            logDebug("Client added to channel " + name);
            channel->addOperator(&client);
            logDebug("Client added as operator to channel " + name);
        } else {
            channel = server.get_channels()[name];
            // verifica se o usuario ja eh membro desse canal
            if (channel->isMember(&client)) {
                logDebug("Client already member of channel " + name);
                client.sendReply(ERR_USERONCHANNEL(client.getNickname(), channel->getName()));
                continue;
            }

            // verifica limite de usuarios (+l), sai daqui. nao entra mais ninguem
            if (channel->hasMode(Channel::LIMIT_SET) &&
                (int)channel->getMembers().size() >= channel->getLimit()) {
                logDebug("Channel " + name + " reached user limit");
                client.sendReply(ERR_CHANNELISFULL(channel->getName()));
                continue;
            }

            // verifica se o canal eh invite only (+i) nao foi convidado, sai daqui
            if (channel->hasMode(Channel::INVITE_ONLY) && !channel->isInvited(&client)) {
                logDebug("Client not invited to channel " + name);
                client.sendReply(ERR_INVITEONLYCHAN(channel->getName()));
                continue;
            }
            // verifica se precisa de senha (+k) senha errada? sai daqui
            if (channel->hasMode(Channel::KEY_REQUIRED)) {
                if (i >= modes.size() || channel->getKey() != modes[i]) {
                    logDebug("Invalid key for channel " + name);
                    client.sendReply(
                        ERR_BADCHANNELKEY(client.getUsername(), channel->getName()));
                    continue;
                }
            }
        }
        // adiciona o usuario como membro do canal nada de errado com os modes? entao entra
        channel->addMember(&client);
        logDebug("Client added to channel " + name);

        // broadcast do JOIN para todos membros
        std::string joinMsg = ":" + client.getPrefix() + " JOIN :" + name;
        logDebug("Broadcast JOIN -> " + joinMsg);
        channel->broadcast(joinMsg);

        // remove o convite desse usuario da lista de convidados
        if (channel->isInvited(&client)) {
            logDebug("Removing invite for client " + client.getNickname());
            channel->removeInvite(&client);
        }

        // envia informacoes do topico do canal
        std::string topic = channel->getTopic();
        if (topic.empty()) {
            logDebug("Channel " + name + " has no topic");
            client.sendReply(RPL_NOTOPIC(client.getNickname(), channel->getName()));
        } else {
            logDebug("Channel " + name + " topic -> " + topic);
            client.sendReply(RPL_TOPIC(client.getNickname(), channel->getName(), topic));
        }

        // monta a lista de usuarios do canal
        std::string           namesReply = "=";
        std::vector<Client *> members    = channel->getMembers();
        std::stringstream     ss_members;
        ss_members << "Channel members " << name << ":";
        for (size_t j = 0; j < members.size(); ++j) {
            if (channel->isOperator(members[j])) {
                namesReply += " @" + members[j]->getNickname();
                ss_members << " @" << members[j]->getNickname();
            } else {
                namesReply += " " + members[j]->getNickname();
                ss_members << " " << members[j]->getNickname();
            }
        }
        logDebug(ss_members.str());

        // envia a lista de usuarios
        client.sendReply(RPL_NAMREPLY(client.getNickname(), channel->getName(), namesReply));
        client.sendReply(RPL_ENDOFNAMES(client.getNickname(), channel->getName()));
    }
}

bool Commands::isValidChannelName(const std::string &name) {
    if (name[0] != '#' && name[0] != '&')
        return false;

    for (size_t i = 1; i < name.size(); ++i) {
        unsigned char c = name[i];
        if (std::isspace(c)) // espaço
            return false;
        if (c == ':') // caractere especial proibido
            return false;
        if (std::iscntrl(c)) // caracteres de controle
            return false;
    }
    return true;
}

bool Commands::isValidkey(std::string key) {
    for (size_t i = 1; i < key.size(); ++i) {
        unsigned char c = key[i];
        if (std::isspace(c)) // espaço
            return false;
        if (c == ':') // caractere especial proibido
            return false;
        if (std::iscntrl(c)) // caracteres de controle
            return false;
    }
    return true;
}
