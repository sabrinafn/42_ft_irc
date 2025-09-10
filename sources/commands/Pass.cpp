#include "../includes/ft_irc.hpp"

/* HANDLEPASS */
void Commands::handlePass(Client &client, Server &server, const IRCMessage &msg) {
    if (client.getState() != UNREGISTERED) {
        client.sendReply(ERR_ALREADYREGISTERED(client.getNickname()));
        return;
    }
    if (msg.params.empty()) {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }
    if (msg.params[0] == server.getServerPassword()) {
        client.setState(PASS_OK);
        std::stringstream ss;
        ss << "Client [" << client.getFd() << "] provided a correct password";
        logInfo(ss.str());
    } else {
        client.sendReply(ERR_PASSWDMISMATCH());
        std::stringstream ss2;
        ss2 << "Client [" << client.getFd() << "] provided an incorrect password";
        logError(ss2.str());
    }
}
