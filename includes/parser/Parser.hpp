/*
 * Parser.hpp
 *
 * Defines the Parser class for parsing IRC messages according to RFC 1459.
 * Handles message buffering, line extraction, and command/parameter parsing.
 */
#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

struct IRCMessage {
    std::string              prefix;  // usually server name or nick!user@host
    std::string              command; // Command (NICK, USER, PRIVMSG, etc....)
    std::vector<std::string> params;
    std::string              trailing; // Optional parameter (after :)

    IRCMessage() {
    }
    IRCMessage(const std::string& cmd) : command(cmd) {
    }
};

class Parser {
   public:
    Parser();
    Parser(const Parser& other);
    Parser& operator=(const Parser& other);
    ~Parser();

    static IRCMessage               parseMessage(const std::string& line);
    static std::vector<std::string> extractLines(std::string& buffer);
    static bool                     isValidCommand(const std::string& command);
    static bool                     isValidNickname(const std::string& nickname);

   private:
    static std::string              trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
};

#endif
