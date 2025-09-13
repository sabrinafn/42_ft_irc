#include "../includes/ft_irc.hpp"

void Commands::handleNames(Client &client, Server &server, const IRCMessage &msg) {
    std::vector<std::string> requestedChannels;

    if (!msg.params.empty()) {
        std::stringstream ss(msg.params[0]);
        std::string       channelName;
        while (std::getline(ss, channelName, ','))
            if (!channelName.empty())
                requestedChannels.push_back(channelName);
    }

    std::vector<std::string> channelstoPrint;

    if (requestedChannels.empty()) { // printing all channels
        std::map<std::string, Channel *>::iterator it = server.get_channels().begin();
        while (it != server.get_channels().end()) {
            Channel *current = it->second;
            channelstoPrint.push_back(current->getName());
            it++;
        }
    } else { // print only requestedChannels
        for (size_t i = 0; i < requestedChannels.size(); ++i) {
            const std::string &name = requestedChannels[i];
            if (server.get_channels().find(name) != server.get_channels().end())
                channelstoPrint.push_back(name);
        }
    }

    // get and send each channel members
    for (size_t i = 0; i < channelstoPrint.size(); ++i) {
        Channel *current = server.get_channels()[channelstoPrint[i]];
        sendChannelMembers(client, current);
    }

    // users not in any channel
    if (requestedChannels.empty()) {
        std::string                             clientsList;
        std::map<int, Client *>::const_iterator it = server.getClients().begin();
        while (it != server.getClients().end()) {
            Client *current   = it->second;
            bool    inChannel = false;
            for (size_t i = 0; i < channelstoPrint.size(); ++i) {
                if (server.get_channels()[channelstoPrint[i]]->isMember(current)) {
                    inChannel = true;
                    break;
                }
            }
            if (!inChannel) {
                clientsList += " " + current->getNickname();
            }
            it++;
        }
        if (!clientsList.empty()) {
            client.sendReply(RPL_NAMREPLY(client.getNickname(), "*", clientsList));
            client.sendReply(RPL_ENDOFNAMES(client.getNickname(), "*"));
        }
    }
}

void Commands::sendChannelMembers(Client &client, Channel *channel) {
    std::string           membersList = "=";
    std::vector<Client *> members     = channel->getMembers();

    for (size_t i = 0; i < members.size(); ++i) {
        if (channel->isOperator(members[i]))
            membersList += " @" + members[i]->getNickname();
        else
            membersList += " " + members[i]->getNickname();
    }
    client.sendReply(RPL_NAMREPLY(client.getNickname(), channel->getName(), membersList));
    client.sendReply(RPL_ENDOFNAMES(client.getNickname(), channel->getName()));
}
