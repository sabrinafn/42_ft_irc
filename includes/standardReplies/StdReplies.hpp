#ifndef STDREPLIES_HPP
#define STDREPLIES_HPP

#include <string>
#include <sstream>
extern const std::string SERVER;
extern const char        SPACE;
extern const char        COLON;
extern const std::string CRLF;

enum IRCReplyCode {
    RPL_WELCOME_CODE          = 001,
    RPL_YOURHOST_CODE         = 002,
    RPL_CREATED_CODE          = 003,
    RPL_MYINFO_CODE           = 004,
    RPL_NOTOPIC_CODE          = 331,
    RPL_TOPIC_CODE            = 332,
    RPL_NAMREPLY_CODE         = 353,
    RPL_ENDOFNAMES_CODE       = 366,
    ERR_UNKNOWNCMD_CODE       = 421,
    ERR_NONICKNAMEGIVEN_CODE  = 431,
    ERR_ERRONEUSNICKNAME_CODE = 432,
    ERR_NICKNAMEINUSE_CODE    = 433,
    ERR_USERONCHANNEL_CODE    = 443,
    ERR_NEEDMOREPARAMS_CODE   = 461,
    ERR_ALREADYREGISTRED_CODE = 462,
    ERR_PASSWDMISMATCH_CODE   = 464,
    ERR_CHANNELISFULL_CODE    = 471,
    ERR_INVITEONLYCHAN_CODE   = 473,
    ERR_BADCHANNELKEY_CODE    = 475,
    ERR_NOTEXTTOSEND_CODE     = 412,
    ERR_NOSUCHCHANNEL_CODE    = 403,
    ERR_NOTONCHANNEL_CODE     = 442,
    ERR_NOSUCHNICK_CODE       = 406,
    ERR_CHANOPRIVSNEEDED_CODE = 482,
    ERR_USERNOTINCHANNEL_CODE = 441,
    RPL_INVITING_CODE         = 341
};

// 001
std::string RPL_WELCOME(const std::string& nickname, const std::string& username);

// 002
std::string RPL_YOURHOST(const std::string& nickname);

// 003
std::string RPL_CREATED(const std::string& nickname, const std::string& startup_time);

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

// 464 alternate text for ordering: Password required
std::string ERR_PASSREQUIRED(void);

// 471
std::string ERR_CHANNELISFULL(const std::string& channel);

// 473
std::string ERR_INVITEONLYCHAN(const std::string& channel);

// 475
std::string ERR_BADCHANNELKEY(const std::string& username, const std::string& channel);

// 412
std::string ERR_NOTEXTTOSEND(const std::string& nickname);

// 403
std::string ERR_NOSUCHCHANNEL(const std::string& channel);

// 442
std::string ERR_NOTONCHANNEL(const std::string& channel);

// 406
std::string ERR_NOSUCHNICK(const std::string& dest);

// 482
std::string ERR_CHANOPRIVSNEEDED(const std::string& nickname, const std::string& channel);

// 441
std::string ERR_USERNOTINCHANNEL(const std::string& targetNick, const std::string& nickname,
                                 const std::string& channel);

// 341
std::string RPL_INVITING(const std::string& targetNick, const std::string& nickname,
                         const std::string& channel);

std::string RPL_INVITE(const std::string& targetNick, const std::string& nickname,
                       const std::string& channel);

// 472
std::string ERR_UNKNOWNMODE(const std::string& mode);

std::string RPL_CHANNELMODEIS(const std::string& channel, const std::string& mode,
                              const std::string& modeParams);
// PRIVMSG
std::string RPL_PRIVMSG(const std::string& prefix, const std::string& other,
                        const std::string& message);
#endif
