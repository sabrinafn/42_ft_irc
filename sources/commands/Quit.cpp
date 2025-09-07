#include "../includes/ft_irc.hpp"

/* HANDLEQUIT */
void Commands::handleQuit(Client &client, Server &server, const IRCMessage &msg) {
    std::string quitMessage = msg.trailing.empty() ? "Client Quit" : msg.trailing;

    // CRITICAL: Remove client from all channels BEFORE disconnecting
    std::map<std::string, Channel*> &channels = server.get_channels();
    std::vector<std::string> channelsToRemove;
    
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        Channel* channel = it->second;
        if (channel->isMember(&client))
        {
            std::string quitMsg = ":" + client.getPrefix() + " QUIT :" + quitMessage;
            channel->broadcast(quitMsg, &client);
            channel->removeMember(&client);
            channel->removeOperator(&client);
            channel->removeInvite(&client);
            
            if (channel->isEmptyChannel())
            {
                channelsToRemove.push_back(channel->getName());
            }
        }
    }
    for (size_t i = 0; i < channelsToRemove.size(); ++i)
    {
        server.removeChannel(channelsToRemove[i]);
    }
    // Send quit message to client (optional)
    std::stringstream ss;
    ss << "Client [" << client.getFd() << "] (" << client.getNickname()
       << ") quit: " << quitMessage;
    logInfo(ss.str());

    // Find the client in pollset and disconnect
    size_t pollIndex = server.getPollsetIdxByFd(client.getFd());
    if (pollIndex != static_cast<size_t>(-1)) {
        server.disconnectClient(pollIndex);
    }
}
