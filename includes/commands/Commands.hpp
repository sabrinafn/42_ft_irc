#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <map>
#include "../server/Server.hpp"
#include "../client/Client.hpp"
#include "../parser/Parser.hpp"

class Server;
class Client;

class Commands {
   private:
    // typedef for standard function type
    typedef void (Commands::*CommandFunc)(Client &, Server &, const IRCMessage &);
    // map to store commands names and commands functions
    std::map<std::string, CommandFunc> commandsMap;

    void handlePass(Client &client, Server &server, const IRCMessage &msg);
    void handleNick(Client &client, Server &server, const IRCMessage &msg);
    void handleUser(Client &client, Server &server, const IRCMessage &msg);
    void handlePing(Client &client, Server &server, const IRCMessage &msg);
    void handlePong(Client &client, Server &server, const IRCMessage &msg);
    void handleQuit(Client &client, Server &server, const IRCMessage &msg);
    void handleJoin(Client &client, Server &server, const IRCMessage &msg);
    void handlePrivmsg(Client &client, Server &server, const IRCMessage &msg);
    void handleTopic(Client &client, Server &server, const IRCMessage &msg);
    void handleKick(Client &client, Server &server, const IRCMessage &msg);
    void handleInvite(Client &client, Server &server, const IRCMessage &msg);
    void handleMode(Client &client, Server &server, const IRCMessage &msg);
    void handleNames(Client &client, Server &server, const IRCMessage &msg);
    void handleJoke(Client &client, Server &server, const IRCMessage &msg);
    void handleTime(Client &client, Server &server, const IRCMessage &msg);

    /* UTILS FUNCTIONS */

    /* CHECK IF NICKNAME IS ALREADY IN USE */
    bool isNicknameInUse(Server &server, const std::string &nickname, int excludeFd = -1);

    /* CHECK IF IT IS A VALID CHANNEL NAME */
    bool isValidChannelName(const std::string &name);

    /* CHECK IF IT IS A VALID KEY */
    bool isValidkey(std::string key);

    bool sendMsgToClient(Client &client, Server &server, const IRCMessage &msg);
    bool sendMsgToChannel(Client &client, Server &server, const IRCMessage &msg);

    std::string getChannelNames(Channel *channel);
    void        sendChannelMembers(Client &client, Channel *channel);

   public:
    /* CONSTRUCTOR */
    Commands(void);
    /* COPY CONSTRUCTOR */
    Commands(const Commands &other);
    /* = OPERATOR */
    Commands &operator=(const Commands &other);
    /* DESTRUCTOR */
    ~Commands();

    /* COMMAND HANDLER THAT WILL CALL EACH COMMAND */
    void handler(Client &client, Server &server, const IRCMessage &msg);
};

#endif
