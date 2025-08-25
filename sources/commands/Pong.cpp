#include "../includes/ft_irc.hpp"

void Commands::handlePong(Client &client, Server &server, const IRCMessage &msg) {
    (void)msg; // Suppress unused parameter warning
    if (std::time(0) - client.getLastPingSent() < server.getPongTimeout()) {
        client.setPingSent(false);
        client.setLastActivity(std::time(0));
        std::cout << "PONG received from Client with fd [" << client.getFd() << "]" << std::endl;
    }
    client.setLastActivity(std::time(0));
}
