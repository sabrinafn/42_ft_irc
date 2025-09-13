#include "../includes/ft_irc.hpp"

/* HANDLEKICK */
// /KICK <#canal> <nick> [motivo]
void Commands::handleKick(Client& client, Server& server, const IRCMessage& msg) {
    if (msg.params.empty() || msg.params.size() < 2) {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }

    std::vector<std::string> _channels = split(msg.params[0], ',');
    std::vector<std::string> targets   = split(msg.params[1], ',');

    // Motivo (se houver)
    std::string reason;
    if (msg.params.size() > 2) {
        for (size_t i = 2; i < msg.params.size(); ++i) {
            reason += msg.params[i];
            if (i != msg.params.size() - 1)
                reason += " ";
        }
    } else {
        reason = client.getNickname();
    }

    for (size_t i = 0; i < _channels.size(); ++i) {
        std::string channelName = _channels[i];
        logDebug("KICK: channel name: " + channelName);

        std::map<std::string, Channel*>& all_channels = server.get_channels();

        if (!server.hasChannel(channelName)) {
            client.sendReply(ERR_NOSUCHCHANNEL(channelName));
            continue;
        }

        Channel* channel = all_channels[channelName];
        if (!channel->isMember(&client)) {
            client.sendReply(ERR_NOTONCHANNEL(channelName));
            continue;
        }

        if (!channel->isOperator(&client)) {
            client.sendReply(ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName));
            continue;
        }

        for (size_t ti = 0; ti < targets.size(); ++ti) {
            std::string targetNick = targets[ti];
            Client*     target     = server.getClientByNick(targetNick);

            if (!target) {
                client.sendReply(ERR_NOSUCHNICK(targetNick));
                continue;
            }

            if (!channel->isMember(target)) {
                client.sendReply(
                    ERR_USERNOTINCHANNEL(targetNick, client.getNickname(), channelName));
                continue;
            }
            if (client.getFd() == target->getFd()) {
                client.sendReply(ERR_USERCANNOTKICKITSELF(client.getNickname(), channelName));
                continue;
            }

            std::string kickMsg = ":" + client.getPrefix() + " KICK " + channelName + " " +
                                  targetNick + " :" + reason;
           
            channel->broadcast(kickMsg, &client);

            channel->removeMember(target);
            channel->removeOperator(target);

        }
    }
}
