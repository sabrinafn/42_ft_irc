#ifndef STDREPLIES_HPP
#define STDREPLIES_HPP

#include <string>
extern const std::string SERVER;

// 001
std::string RPL_WELCOME(const std::string& nickname, const std::string& username);

// 002
std::string RPL_YOURHOST(const std::string& nickname);

// 003
std::string RPL_CREATED(const std::string& nickname, std::string& startup_time);

// 004
std::string RPL_MYINFO(const std::string& nickname, const std::string& usermodes,
                       const std::string& channelmodes);

// 331
std::string RPL_NOTOPIC(const std::string& nickname, const std::string& channel);

// 332
std::string RPL_TOPIC(const std::string& nickname, const std::string& channel,
                      const std::string& topic);

// 353
std::string RPL_NAMREPLY(const std::string& nickname, const std::string& channel,
                         const std::string& members_list);

// 366
std::string RPL_ENDOFNAMES(const std::string& nickname, const std::string& channel);

// 421
std::string ERR_UNKNOWNCMD(const std::string& command);

// 431
std::string ERR_NONICKNAMEGIVEN(void);

// 432
std::string ERR_ERRONEUSNICKNAME(const std::string& nickname);

// 433
std::string ERR_NICKNAMEINUSE(const std::string& nickname);

// 443
std::string ERR_USERONCHANNEL(const std::string& nickname, const std::string& channel);

// 461
std::string ERR_NEEDMOREPARAMS(const std::string& command);

// 462
std::string ERR_ALREADYREGISTRED(const std::string& nickname);

// 464
std::string ERR_PASSWDMISMATCH(void);

// 471
std::string ERR_CHANNELISFULL(const std::string& channel);

// 473
std::string ERR_INVITEONLYCHAN(const std::string& channel);

// 475
std::string ERR_BADCHANNELKEY(const std::string& username, const std::string& channel);

// PRIVMSG
std::string RPL_PRIVMSG(const std::string& nickname, const std::string& other,
                        const std::string& message);

// 412
std::string ERR_NOTEXTTOSEND(const std::string& nickname);

// 403
std::string ERR_NOSUCHCHANNEL(const std::string& channel);

// 442
std::string ERR_NOTONCHANNEL(const std::string& channel);

// 406
std::string ERR_NOSUCHNICK(const std::string& dest);

// 482
std::string ERR_CHANOPRISNEEDED(const std::string& nickname, const std::string& channel);

// 441
std::string ERR_USERNOTINCHANNEL(const std::string& targetNick, const std::string& nickname,
                                 const std::string& channel);

// invite
std::string RPL_INVITING(const std::string& targetNick, const std::string& nickname,
                         const std::string& channel);

std::string RPL_INVITE(const std::string& targetNick, const std::string& nickname,
                       const std::string& channel);

#endif
