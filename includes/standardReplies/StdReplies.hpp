#ifndef STDREPLIES_HPP
# define STDREPLIES_HPP

#include <string>

const std::string SERVER = "<3_irc_server_<3";

// 001
std::string RPL_WELCOME(const std::string& nickname, const std::string& username);

// 002
std::string RPL_YOURHOST(const std::string& nickname);

// 003
std::string RPL_CREATED(const std::string& nickname, std::string& startup_time);

// 004
std::string RPL_MYINFO(const std::string& nickname, const std::string& usermodes, const std::string& channelmodes);

#endif