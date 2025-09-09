#include "../includes/ft_irc.hpp"

/* HANDLEUSER */
void Commands::handleUser(Client &client, Server &server, const IRCMessage &msg) {
    (void)server;
    if (client.getState() == UNREGISTERED) {
        client.sendReply(ERR_PASSREQUIRED());
        std::stringstream ss;
        ss << "Client [" << client.getFd() << "] not registered yet, sending ERR_PASSREQUIRED";
        logError(ss.str());
        return;
    }

    if (!client.getUsername().empty()) {
        std::stringstream ss1;
        ss1 << "Client [" << client.getFd()
            << "] already has username, sending ERR_ALREADYREGISTRED";
        logError(ss1.str());
        client.sendReply(ERR_ALREADYREGISTRED(client.getNickname()));
        return;
    }

    if (msg.params.size() < 3) {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        std::stringstream ss2;
        ss2 << "Client [" << client.getFd()
            << "] missing parameters, sending ERR_NEEDMOREPARAMS";
        logInfo(ss2.str());
        return;
    }
    if (msg.params[0].empty()) {
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        std::stringstream ss_empty;
        ss_empty << "Client [" << client.getFd()
                 << "] provided empty username, sending ERR_NEEDMOREPARAMS";
        logInfo(ss_empty.str());
        return;
    }
    if (!msg.trailing.empty())
        client.setRealname(msg.trailing); // no realname is accepted
    client.setUsername(msg.params[0]);
    std::stringstream ss3;
    ss3 << "Client [" << client.getFd() << "] set username: " << client.getUsername();
    logInfo(ss3.str());

    if (client.getState() == PASS_OK && !client.getNickname().empty()) {
        client.setState(REGISTERED);
        std::stringstream ss4;
        ss4 << "Client [" << client.getFd() << "] is now REGISTERED, sending welcome messages";
        logInfo(ss4.str());
        client.sendWelcomeMessages();
    }
}
