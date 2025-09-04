#include "../includes/ft_irc.hpp"

/* HANDLEPRIVMSG */

bool Commands::sendMsgToChannel(Client &client, Server &server, const IRCMessage &msg) {
    Channel                         *channel;
    std::string                      dest         = msg.params[0];
    std::map<std::string, Channel *> all_channels = server.get_channels();
    if (all_channels.find(dest) != all_channels.end()) {
        channel = all_channels[dest];
        if (!channel->isMember(&client)) {
            client.sendReply(ERR_NOTONCHANNEL(dest));
            return false;
        }
        std::string response = RPL_PRIVMSG(client.getPrefix(), dest, msg.trailing);
        channel->broadcast(response, &client);
    } else {
        client.sendReply(ERR_NOSUCHCHANNEL(dest));
        return false;
    }

    return true;
}

bool Commands::sendMsgToClient(Client &client, Server &server, const IRCMessage &msg) {
    std::string dest = msg.params[0];

    const std::vector<Client *> &serverClients =
        server.getClients(); // pega a lista de clientes do server
    for (size_t i = 0; i < serverClients.size(); ++i) {
        if (serverClients[i]->getNickname() == dest) {
            std::string fullMessage = RPL_PRIVMSG(client.getPrefix(), dest, msg.trailing);
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

    if (msg.params.empty() || msg.trailing.empty()) {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }
    if (msg.params[0][0] == '#')
    {
        if (sendMsgToChannel(client, server, msg))
            return;
    }
    else{
        if (sendMsgToClient(client, server, msg))
            return;
    }       
    return;
}
