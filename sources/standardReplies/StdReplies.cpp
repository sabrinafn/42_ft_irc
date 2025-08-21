#include "../includes/standardReplies/StdReplies.hpp"

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
