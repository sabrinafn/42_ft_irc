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
    typedef void (Commands::*CommandFunc)(Client &, Server &, const IRCMessage &);
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


    bool isNicknameInUse(Server &server, const std::string &nickname, int excludeFd = -1);

    bool isValidChannelName(const std::string &name);

    bool isValidkey(std::string key);

    bool sendMsgToClient(Client &client, Server &server, const IRCMessage &msg);
    bool sendMsgToChannel(Client &client, Server &server, const IRCMessage &msg);

    std::string getChannelNames(Channel *channel);
    void        sendChannelMembers(Client &client, Channel *channel);

   public:
 
    Commands(void);
    Commands(const Commands &other);
    Commands &operator=(const Commands &other);
    ~Commands();

    void handler(Client &client, Server &server, const IRCMessage &msg);
};

#endif
