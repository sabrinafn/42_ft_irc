#include "../includes/ft_irc.hpp"

/* HANDLENICK */
void Commands::handleNick(Client &client, Server &server, const IRCMessage &msg) {
    if (msg.params.empty()) {
        logError("No nickname provided, sending ERR_NONICKNAMEGIVEN");
        client.sendReply(ERR_NONICKNAMEGIVEN());
        return;
    }

    std::string nickname = msg.params[0];
    if (!Parser::isValidNickname(nickname)) {
        logError("Invalid nickname: " + nickname + ", sending ERR_ERRONEUSNICKNAME");
        client.sendReply(ERR_ERRONEUSNICKNAME(nickname));
        return;
    }

    // Verifica se o nickname já está em uso
    if (isNicknameInUse(server, nickname, client.getFd())) {
        logError("Nickname already in use: " + nickname + ", sending ERR_NICKNAMEINUSE");
        client.sendReply(ERR_NICKNAMEINUSE(nickname));
        return;
    }

    client.setNickname(nickname);

    std::stringstream ss;
    ss << "Client [" << client.getFd() << "] set nickname to: " << client.getNickname();
    logInfo(ss.str());

    if (client.getState() == PASS_OK && !client.getUsername().empty()) {
        client.setState(REGISTERED);
        std::stringstream ss1;
        ss1 << "Client [" << client.getFd() << "] is now REGISTERED, sending welcome messages";
        logInfo(ss1.str());
        client.sendWelcomeMessages();
    }
}

/* CHECK IF NICKNAME IS ALREADY IN USE */
bool Commands::isNicknameInUse(Server &server, const std::string &nickname, int excludeFd) {
    const std::map<int, Client *> &clients = server.getClients();
    for (std::map<int, Client *>::const_iterator it = clients.begin(); it != clients.end();
         ++it) {
        if (it->first != excludeFd && it->second->getNickname() == nickname) {
            return true;
        }
    }
    return false;
}
