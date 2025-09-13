#include "../includes/ft_irc.hpp"

void Commands::handlePong(Client &client, Server &server, const IRCMessage &msg) {
    (void)msg; 
    if (std::time(0) - client.getLastPingSent() < server.getPongTimeout()) {
        client.setPingSent(false);
        client.setLastActivity(std::time(0));
        std::stringstream ss;
        ss << "PONG received from Client [" << client.getFd() << "]";
        logDebug(ss.str());
    }
    client.setLastActivity(std::time(0));
}
