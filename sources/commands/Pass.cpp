#include "../includes/ft_irc.hpp"

/* HANDLEPASS */
void Commands::handlePass(Client &client, Server &server, const IRCMessage &msg) {
    if (client.getState() != UNREGISTERED) {
        client.sendReply(ERR_ALREADYREGISTRED(client.getNickname()));
        return;
    }    
    if (msg.params.empty()) {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }
    if (msg.params[0] == server.getServerPassword()) {
        client.setState(PASS_OK);
        std::cout << "DEBUG: Client " << client.getFd() << " provided correct password" << std::endl;
    } else {
        client.sendReply(ERR_PASSWDMISMATCH());
        std::cout << "ERROR: Client " << client.getFd() << " provided an incorrect password" << std::endl;
    }
}
