#ifndef REPLIES_HPP
#define REPLIES_HPP

#define SERVER std::string(":<3_irc_server_<3")
#define SERVER2 std::string("<3_irc_server_<3")
#define CRLF std::string("\r\n")
#define COLON std::string(":")
#define SPACE std::string(" ")

#define RPL_WELCOME(nickname, userPrefix) \
    (SERVER + " 001 " + nickname + " :Welcome to the Internet Relay Chat " + userPrefix + CRLF)
#define RPL_YOURHOST(nickname)                                                             \
    (SERVER + " 002 " + nickname + " :Your host is " + SERVER2 + ", running version 1.0" + \
     CRLF)
#define RPL_CREATED(nickname, startup_time) \
    (SERVER + " 003 " + nickname + " :This server was created on " + startup_time + CRLF)
#define RPL_MYINFO(nickname, usermodes, channelmodes)                              \
    (SERVER + " 004 " + nickname + SPACE + SERVER2 + " 1.0 " + usermodes + SPACE + \
     channelmodes + CRLF)
#define RPL_CHANNELMODEIS(channel, mode, modeParams) \
    (SERVER + " 324 * " + channel + SPACE + mode + SPACE + modeParams + CRLF)
#define RPL_NOTOPIC(nickname, channel) \
    (SERVER + " 331 " + nickname + SPACE + channel + " :No topic is set" + CRLF)
#define RPL_TOPIC(nickname, channel, topic) \
    (SERVER + " 332 " + nickname + SPACE + channel + " :" + topic + CRLF)
#define RPL_INVITING(targetNick, nickname, channel) \
    (SERVER + " 341 " + nickname + SPACE + targetNick + SPACE + channel + CRLF)
#define RPL_NAMREPLY(nickname, channel, names) \
    (SERVER + " 353 " + nickname + " = " + channel + " :" + names + CRLF)
#define RPL_ENDOFNAMES(nickname, channel) \
    (SERVER + " 366 " + nickname + SPACE + channel + " : End of names list" + CRLF)
#define ERR_NOSUCHNICK(targetNick) \
    (SERVER + " 401 " + targetNick + " :No such nick" + CRLF)
#define ERR_NOSUCHCHANNEL(channel) \
    (SERVER + " 403 * " + channel + " :No such channel" + CRLF)
#define ERR_NOORIGIN(nickname) \
    (SERVER + " 409 " + nickname + " :No origin specified" + CRLF)
#define ERR_NOTEXTTOSEND(nickname) \
    (SERVER + " 412 " + nickname + " :No text to send" + CRLF)
#define ERR_UNKNOWNCOMMAND(nickname, command) \
    (SERVER + " 421 " + nickname + SPACE + command + " :Unknown command" + CRLF)
#define ERR_NONICKNAMEGIVEN() \
    (SERVER + " 431 * :No nickname given" + CRLF)
#define ERR_ERRONEUSNICKNAME(nickname) \
    (SERVER + " 432 * " + nickname + " :Erroneus nickname" + CRLF)
#define ERR_NICKNAMEINUSE(nickname) \
    (SERVER + " 433 * " + nickname + " :Nickname is already in use" + CRLF)
#define ERR_USERNOTINCHANNEL(targetNick, nickname, channel)               \
    (SERVER + " 441 " + nickname + SPACE + targetNick + SPACE + channel + \
     " :They aren't on that channel" + CRLF)
#define ERR_NOTONCHANNEL(channel) \
    (SERVER + " 442 * " + channel + " :You're not on that channel" + CRLF)
#define ERR_USERONCHANNEL(nickname, channel) \
    (SERVER + " 443 * " + nickname + SPACE + channel + " :is already on channel" + CRLF)
#define ERR_NOTREGISTERED(command) \
    (SERVER + " 451 * " + command + " :You have not registered" + CRLF)
#define ERR_NEEDMOREPARAMS(command) \
    (SERVER + " 461 * " + command + " :Not enough parameters" + CRLF)
#define ERR_ALREADYREGISTERED(nickname) \
    (SERVER + " 462 " + nickname + " :You may not reregister" + CRLF)
#define ERR_PASSWDMISMATCH() (SERVER + " 464 * :Password incorrect" + CRLF)
#define ERR_KEYSET(nickname, channel) \
    (SERVER + " 467 " + nickname + SPACE + channel + " :Channel key already set" + CRLF)
#define ERR_CHANNELISFULL(channel) \
    (SERVER + " 471 * " + channel + " :Cannot join channel (+l)" + CRLF)
#define ERR_UNKNOWNMODE(nickname, mode) \
    (SERVER + " 472 * " + nickname + SPACE + mode + " :is unknown mode char to me" + CRLF)
#define ERR_INVITEONLYCHAN(channel) \
    (SERVER + " 473 * " + channel + " :Cannot join channel (+i)" + CRLF)
#define ERR_BADCHANNELKEY(nickname, channel) \
    (SERVER + " 475 *" + nickname + SPACE + channel + " :Cannot join channel (+k)" + CRLF)
#define ERR_CHANOPRIVSNEEDED(nickname, channel) \
    (SERVER + " 482 " + nickname + SPACE + channel + " :You're not a channel operator" + CRLF)

#define JOIN(client_prefix, channel) (COLON + client_prefix + " JOIN :" + channel + CRLF)
#define RPL_PRIVMSG(nickname, dest, message) \
    (COLON + nickname + " PRIVMSG " + dest + " :" + message + CRLF)
#define RPL_INVITE(targetNick, nickname, channel) \
    (SERVER + SPACE + nickname + " INVITE " + targetNick + SPACE + channel + CRLF)
#define RPL_PONG(token) (SERVER + " PONG " + token + "\r\n")
#define ERR_SERVERSHUTDOWN(nickname) \
    (SERVER + " ERROR :Closing Link: " + nickname + " (Server shutting down)" + CRLF)
#define ERR_USERCANNOTKICKITSELF(nickname, channel) (SERVER + SPACE + nickname + SPACE + channel + ":You cannot kick yourself from the channel")

#endif
