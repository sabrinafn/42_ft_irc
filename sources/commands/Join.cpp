#include "../includes/ft_irc.hpp"

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
            channel->addOperator(&client);}
        // } else {
        //     //channel = channels[name];
        //     std::cout << "ERROR: canal ja existe " << name << std::endl;
        // }

        // verifica se o usuario ja eh membro desse canal
        std::vector<Client*> members = channel->getMembers();
 
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



