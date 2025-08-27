#include "../includes/ft_irc.hpp"

/* HANDLEPRIVMSG */

std::string Commands::buildMessageFromParams(const std::vector<std::string> &params) {
    std::string message;

    for (size_t i = 1; i < params.size(); ++i) {
        message += params[i];
        if (i != params.size() - 1)
            message += " ";
    }
    if (!message.empty() && message[0] == ':')
        message = message.substr(1);

    return message;
}

bool Commands::check_params(Client &client, const IRCMessage &msg) {
    // colocar if de autenticação

    if (msg.params.empty() || msg.params.size() < 2) {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        std::cout << "ERROR: returning ERR_NEEDMOREPARAMS for msg.params.empty or "
                     "msg.params.size() < 2"
                  << std::endl;
        return false;
    }
    if (msg.params[1][0] != ':') {
        client.sendReply(ERR_NOTEXTTOSEND(msg.command));
        return false;
    }
    return true;
}

bool Commands::sendMsgToChannel(Client &client, Server &server, const IRCMessage &msg) {
    Channel                         *channel;
    std::string                      dest         = msg.params[0];
    std::map<std::string, Channel *> all_channels = server.get_channels();
    if (dest[0] == '#' && all_channels.find(dest) != all_channels.end()) {
        channel = all_channels[dest];
        if (!channel->isMember(&client)) {
            client.sendReply(ERR_NOTONCHANNEL(dest));
            return false;
        }
        /// ver se é necessario fazer a validação de onlyinvited
        std::string message  = buildMessageFromParams(msg.params);
        std::string response = RPL_PRIVMSG(client.getNickname(), dest, message);
        channel->broadcast(response, &client);
    } else {
        client.sendReply(ERR_NOSUCHCHANNEL(dest));
        return false;
    }

    return true;
}

bool Commands::sendMsgToClient(Client &client, Server &server, const IRCMessage &msg) {
    std::string dest    = msg.params[0];
    std::string message = buildMessageFromParams(msg.params);

    const std::vector<Client *> &serverClients =
        server.getClients(); // pega a lista de clientes do server
    for (size_t i = 0; i < serverClients.size(); ++i) {
        if (serverClients[i]->getNickname() == dest) {
            std::string fullMessage = RPL_PRIVMSG(client.getNickname(), dest, message);
            serverClients[i]->sendReply(fullMessage);
            return true; // encontrou, não precisa continuar
        }
    }
    // se não encontrar o cliente:
    client.sendReply(ERR_NOSUCHNICK(dest));
    // verificar se o erro esta correto
    return false;
}

void Commands::handlePrivmsg(Client &client, Server &server, const IRCMessage &msg) {
    // std::string prefix;      // usually server name or nick!user@host
    std::cout << "DEBUG: msg.prefix: " + msg.prefix << std::endl;
    // std::string command;     // Command (NICK, USER, PRIVMSG, etc....)
    std::cout << "DEBUG: msg.command: " + msg.command << std::endl;
    // std::vector<std::string> params;
    std::cout << "DEBUG: msg.params[0]: " + msg.params[0] << std::endl;
    // std::string trailing;    // Optional parameter (after :)
    std::cout << "DEBUG: msg.trailing: " + msg.trailing << std::endl;

    if (!check_params(client, msg))
        return;
    if (sendMsgToChannel(client, server, msg))
        return;
    if (sendMsgToClient(client, server, msg))
        return;

    return;
}
