#include "../includes/ft_irc.hpp"

/* CONSTRUCTOR */
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
    // commandsMap["MODE"]    = &Commands::handleMode;
}

/* COPY CONSTRUCTOR */
Commands::Commands(const Commands& other) {
    *this = other;
}

/* = OPERATOR */
Commands& Commands::operator=(const Commands& other) {
    (void)other;
    return *this;
}

/* DESTRUCTOR */
Commands::~Commands(void) {}

/* COMMAND HANDLER THAT WILL CALL EACH COMMAND */
void Commands::handler(Client &client, Server &server, const IRCMessage &msg) {
    std::map<std::string, CommandFunc>::iterator it = this->commandsMap.find(msg.command);
    std::cout << "DEBUG: Commands::handler called" << std::endl;
    if (it != commandsMap.end()) {
        CommandFunc func = it->second;
        (this->*func)(client, server, msg);
        std::cout << "DEBUG: command=" << msg.command << std::endl;

    } else {
        std::cout << "ERROR: command=" << msg.command << std::endl;

        if (client.getState() == REGISTERED) {
            client.sendReply(ERR_UNKNOWNCMD(msg.command));
        }
    }
}
