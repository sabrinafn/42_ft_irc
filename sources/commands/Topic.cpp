#include "../includes/ft_irc.hpp"

void Commands::handleTopic(Client &client, Server &server, const IRCMessage &msg)
{
    if (msg.params.empty()) 
    {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }
    std::string                      channelName  = msg.params[0];
    std::map<std::string, Channel *> all_channels = server.get_channels();

    if (all_channels.find(channelName) == all_channels.end()) 
    {
        client.sendReply(ERR_NOSUCHCHANNEL(channelName));
        return;
    }
    Channel *channel = all_channels[channelName];
    
    if (!channel->isMember(&client))
    {
        client.sendReply(ERR_NOTONCHANNEL(channelName));
        return;
    }
    
    bool hasTopicToSet = (msg.params.size() >= 2) || (!msg.trailing.empty());
    
    if (!hasTopicToSet) 
    {
        std::string topic = channel->getTopic();
        if (topic.empty())
        {
            client.sendReply(RPL_NOTOPIC(client.getNickname(), channelName));
            return;
        } else
        {
            client.sendReply(
                RPL_TOPIC(client.getNickname(), channelName, channel->getTopic()));
            return;
        }
    }
    
    std::string newTopic;
    
    if (!msg.trailing.empty())
    {
        newTopic = msg.trailing;
    } else 
    {
        for (size_t i = 1; i < msg.params.size(); ++i)
        {
            newTopic += msg.params[i];
            if (i != msg.params.size() - 1)
                newTopic += " ";
        }
    }

    if (channel->hasMode(Channel::TOPIC_RESTRICTED) && !channel->isOperator(&client))
    {
        client.sendReply(ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName));
        return;
    }

    channel->setTopic(newTopic);

    std::string response = COLON + client.getPrefix() + " TOPIC " + channelName + " :" + newTopic + CRLF;
    channel->broadcast(response, &client);

    return;
}
