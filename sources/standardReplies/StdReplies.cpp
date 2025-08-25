#include "../includes/standardReplies/StdReplies.hpp"

const std::string SERVER = "<3_irc_server_<3";

std::string RPL_WELCOME(const std::string& nickname, const std::string& username) {
  return ":" + SERVER + " 001 " + nickname + " :Welcome to the Internet Relay Chat, " + username + "\r\n";
}

std::string RPL_YOURHOST(const std::string& nickname) {
  return ":" + SERVER + " 002 " + nickname + " :Your host is " + SERVER + ", running version 1.0" + "\r\n";
}

std::string RPL_CREATED(const std::string& nickname, std::string& startup_time) {
  return ":" + SERVER + " 003 " + nickname + " :This server was created on " + startup_time + "\r\n";
}

std::string RPL_MYINFO(const std::string& nickname, const std::string& usermodes, const std::string& channelmodes) {
  return ":" + SERVER + " 004 " + nickname + " " + SERVER + " " + "1.0 " + usermodes + " " + channelmodes + "\r\n";
}

std::string RPL_NOTOPIC(const std::string& nickname, const std::string& channel) {
  return ":" + SERVER + " 331 " + nickname + " " + channel + " :No topic is set" + "\r\n";
}

std::string RPL_TOPIC(const std::string& nickname, const std::string& channel, const std::string& topic) {
  return ":" + SERVER + " 332 " + nickname + " " + channel + " :" + topic + "\r\n";
}

std::string RPL_NAMREPLY(const std::string& nickname, const std::string& channel, const std::string& members_list) {
  return ":" + SERVER + " 353 " + nickname + " = " + channel + " :" + members_list + "\r\n";
}

std::string RPL_ENDOFNAMES(const std::string& nickname, const std::string& channel) {
  return ":" + SERVER + " 366 " + nickname + " " + channel + " :End of names list" + "\r\n";
}

std::string ERR_UNKNOWNCMD(const std::string& command) {
  return ":" + SERVER + " 421 * " + command + " :Unknown command" + "\r\n";
}

std::string ERR_NONICKNAMEGIVEN(void) {
	return ":" + SERVER + " 431 * :No nickname given" + "\r\n";
}

std::string ERR_ERRONEUSNICKNAME(const std::string& nickname) {
  return ":" + SERVER + " 432 *" + nickname + " :Erroneus nickname" + "\r\n";
}

std::string ERR_NICKNAMEINUSE(const std::string& nickname) {
  return ":" + SERVER + " 433 * " + nickname + " :Nickname is already in use" + "\r\n";
}

std::string ERR_USERONCHANNEL(const std::string& nickname, const std::string& channel) {
  return ":" + SERVER + " 443 * " + nickname + " " + channel + " :is already on channel" + "\r\n";
}

std::string ERR_NEEDMOREPARAMS(const std::string& command) {
  return ":" + SERVER + " 461 * " + command + " " + ":Not enough parameters" + "\r\n";
}

std::string ERR_ALREADYREGISTRED(const std::string& nickname) {
  return ":" + SERVER + " 462 * " + nickname + " :You may not reregister" + "\r\n";
}

std::string ERR_PASSWDMISMATCH(void) {
  return ":" + SERVER + " 464 * :Password incorrect" + "\r\n";
}

std::string ERR_CHANNELISFULL(const std::string& channel) {
  return ":" + SERVER + " 471 * " + channel + " :Cannot join channel (+l)" + "\r\n";
}

std::string ERR_INVITEONLYCHAN(const std::string& channel) {
  return ":" + SERVER + " 473 * " + channel + " :Cannot join channel (+i)" + "\r\n";
}

std::string ERR_BADCHANNELKEY(const std::string& username, const std::string& channel) {
  return ":" + SERVER + " 475 * " + username + " " + channel + " :Cannot join channel (+k)" + "\r\n";
}

std::string RPL_PRIVMSG(const std::string& nickname, const std::string& other, const std::string& message) {
  return ":" + SERVER + nickname + " PRIVMSG " + other + " :" + message + "\r\n";
}

std::string ERR_NOTEXTTOSEND(const std::string& nickname) {
  return ":" + SERVER + " 412 * " + nickname + " :No message to send" + "\r\n";
}

std::string ERR_NOSUCHCHANNEL(const std::string& channel) {
  return ":" + SERVER + " 403 * " + channel + " :No such channel" + "\r\n";
}

std::string ERR_NOTONCHANNEL(const std::string& channel) {
  return ":" + SERVER + " 442 * "  + channel + " :Client not on channel!" + "\r\n";
}

std::string ERR_NOSUCHNICK(const std::string& dest) {
  return ":" + SERVER + " 406 * " + dest + " :No such nick" + "\r\n";
}

std::string ERR_CHANOPRISNEEDED(const std::string& nickname, const std::string& channel) {
  return ":" + SERVER + " 482 * " + nickname + channel + " :You're not a channel operator!" + "\r\n";
}

std::string ERR_USERNOTINCHANNEL(const std::string& targetNick, const std::string& nickname, const std::string& channel) {
  return ":" + SERVER + " 441 * " + targetNick + " " + nickname + " " + channel +  " :They aren't on that channel" + "\r\n";
}

std::string RPL_INVITING(const std::string& targetNick, const std::string& nickname, const std::string& channel) {
  return ":" + SERVER + " 341 * "   + nickname + " " + targetNick + " " + channel +  "\r\n";
}

std::string RPL_INVITE(const std::string& targetNick, const std::string& nickname, const std::string& channel) {
  return ":" + SERVER + nickname + " INVITE " + targetNick + " " + channel + "\r\n";
}
