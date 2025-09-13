#include "../includes/ft_irc.hpp"

Commands::Commands(void) {
    commandsMap["PASS"]    = &Commands::handlePass;
    commandsMap["NICK"]    = &Commands::handleNick;
    commandsMap["USER"]    = &Commands::handleUser;
    commandsMap["PING"]    = &Commands::handlePing;
    commandsMap["PONG"]    = &Commands::handlePong;
    commandsMap["QUIT"]    = &Commands::handleQuit;
    commandsMap["JOIN"]    = &Commands::handleJoin;
    commandsMap["PRIVMSG"] = &Commands::handlePrivmsg;
    commandsMap["TOPIC"]   = &Commands::handleTopic;
    commandsMap["KICK"]    = &Commands::handleKick;
    commandsMap["INVITE"]  = &Commands::handleInvite;
    commandsMap["MODE"]    = &Commands::handleMode;
    commandsMap["NAMES"]   = &Commands::handleNames;
    commandsMap["!JOKE"]   = &Commands::handleJoke;
    commandsMap["!TIME"]   = &Commands::handleTime;
}


Commands::Commands(const Commands& other) {
    *this = other;
}


Commands& Commands::operator=(const Commands& other) {
    (void)other;
    return *this;
}


Commands::~Commands(void) {
}


void Commands::handler(Client& client, Server& server, const IRCMessage& msg) {
    std::map<std::string, CommandFunc>::iterator it = this->commandsMap.find(msg.command);
    if (it != commandsMap.end()) {
        if (client.getState() != REGISTERED) {
            if (it->first != "PASS" && it->first != "NICK" && it->first != "USER") {
                client.sendReply(ERR_NOTREGISTERED(it->first));
                return;
            }
        }
        CommandFunc func = it->second;
        (this->*func)(client, server, msg);
    } else {
        if (client.getState() == REGISTERED) {
            client.sendReply(ERR_UNKNOWNCOMMAND(client.getNickname(), msg.command));
        }
    }
}
