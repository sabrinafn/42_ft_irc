#include "../includes/ft_irc.hpp"

void Commands::handlePing(Client &client, Server &server, const IRCMessage &msg) {
    (void)server;
    std::string servername = msg.trailing.empty() ? (msg.params.empty() ? "server" : msg.params[0]) : msg.trailing;
    client.sendReply("PONG :" + servername + "\r\n");
}