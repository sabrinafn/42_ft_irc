#include "../includes/ft_irc.hpp"

/* HANDLENICK */
void Commands::handleNick(Client &client, Server &server, const IRCMessage &msg) {
    if (msg.params.empty()) {
        client.sendReply(ERR_NONICKNAMEGIVEN());
        return;
    }

    std::string nickname = msg.params[0];

    if (!Parser::isValidNickname(nickname)) {
        client.sendReply(ERR_ERRONEUSNICKNAME(nickname));
        return;
    }

    if (isNicknameInUse(server, nickname, client.getFd())) {
        client.sendReply(ERR_NICKNAMEINUSE(nickname));
        return;
    }

    client.setNickname(nickname);
    std::cout << "INFO: Client [" << client.getFd() << "] set nickname to: " << nickname
              << std::endl;

    // Check if client is now fully registered
    if (client.getState() == PASS_OK && !client.getUsername().empty()) {
        client.setState(REGISTERED);
        client.sendWelcomeMessages();
    }
}

/* CHECK IF NICKNAME IS ALREADY IN USE */
bool Commands::isNicknameInUse(Server &server, const std::string &nickname, int excludeFd) {
    const std::vector<Client *> &clients = server.getClients();
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i]->getFd() != excludeFd && clients[i]->getNickname() == nickname) {
            return true;
        }
    }
    return false;
}
