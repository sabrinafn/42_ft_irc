#include "../includes/ft_irc.hpp"



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

    Client *targetClient = server.getClientByNick(dest);

    if (targetClient) {
        std::string fullMessage = RPL_PRIVMSG(client.getPrefix(), dest, msg.trailing);
        targetClient->sendReply(fullMessage);
        return true;
    }
    client.sendReply(ERR_NOSUCHNICK(dest));
    return false;
}

void Commands::handlePrivmsg(Client &client, Server &server, const IRCMessage &msg) {
    if (msg.params.empty()) {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }
    if (msg.trailing.empty()) {
        client.sendReply(ERR_NOTEXTTOSEND(client.getNickname()));
        return;
    }
    if (msg.params[0][0] == '#' || msg.params[0][0] == '&') {
        if (sendMsgToChannel(client, server, msg))
            return;
    } else {
        if (sendMsgToClient(client, server, msg))
            return;
    }
    return;
}
