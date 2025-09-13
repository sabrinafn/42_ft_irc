#include "../includes/ft_irc.hpp"

void Commands::handlePing(Client &client, Server &server, const IRCMessage &msg) {
    (void)server;
    if (msg.params.empty()) {
        client.sendReply(ERR_NOORIGIN(client.getNickname()));
        return;
    }
    client.sendReply(RPL_PONG(msg.params[0]));
}
