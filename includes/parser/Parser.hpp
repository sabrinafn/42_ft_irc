#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

struct IRCMessage {
    std::string              prefix;
    std::string              command;
    std::vector<std::string> params;
    std::string              trailing;

    IRCMessage() {}
    IRCMessage(const std::string& cmd) : command(cmd) {}
};

class Parser {
   public:
    Parser();
    Parser(const Parser& other);
    Parser& operator=(const Parser& other);
    ~Parser();

    struct ParserOptions {
        bool useRFC1459CaseMap;
        ParserOptions() : useRFC1459CaseMap(true) {
        }
    };

    static IRCMessage parseMessage(const std::string& line);
    static IRCMessage parseMessage(const std::string& line, const ParserOptions& options);
    static std::vector<std::string> extractLines(std::string& buffer);
    static bool                     isValidCommand(const std::string& command);
    static bool                     isValidNickname(const std::string& nickname);

   private:
    static std::string trim(const std::string& str);
    static std::string toUpperASCII(const std::string& s);
    static std::string toLowerASCII(const std::string& s);
    static std::string toLowerRFC1459(const std::string& s);
    static std::string foldLower(const std::string& s, bool useRFC1459);

    static void normalizeParamsForCommand(IRCMessage& msg, const ParserOptions& options);
};

#endif
