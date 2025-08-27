#include "../includes/ft_irc.hpp"

/* HANDLEUSER */
void Commands::handleUser(Client &client, Server &server, const IRCMessage &msg) {
    (void)server;
    if (client.getState() == UNREGISTERED) {
        client.sendReply(ERR_PASSREQUIRED());
        return;
    }

    if (!client.getUsername().empty()) {
        client.sendReply(ERR_ALREADYREGISTRED(client.getNickname()));
        return;
    }

    if (msg.params.size() < 3 || msg.trailing.empty()) {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }

    client.setUsername(msg.params[0]);
    client.setRealname(msg.trailing);
    std::cout << "INFO: Client [" << client.getFd() << "] set username: " << msg.params[0]
              << ", realname: " << msg.trailing << std::endl;

    // Check if client is now fully registered
    if (client.getState() == PASS_OK && !client.getNickname().empty()) {
        client.setState(REGISTERED);
        client.sendWelcomeMessages();
    }
}
