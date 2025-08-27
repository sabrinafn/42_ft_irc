#include "../includes/ft_irc.hpp"

Parser::Parser() {
}

Parser::Parser(const Parser& other) {
    *this = other;
}

Parser& Parser::operator=(const Parser& other) {
    (void)other;
    return *this;
}

Parser::~Parser() {
}

std::vector<std::string> Parser::extractLines(std::string& buffer) {
    std::vector<std::string> lines;
    std::string::size_type   pos = 0;

    while ((pos = buffer.find("\r\n")) != std::string::npos) {
        std::string line = buffer.substr(0, pos);
        if (!line.empty()) {
            lines.push_back(line);
        }
        buffer.erase(0, pos + 2); // Remove "\r\n"
    }

    return lines;
}

IRCMessage Parser::parseMessage(const std::string& line) {
    IRCMessage  msg;
    std::string trimmed = trim(line);

    if (trimmed.empty()) {
        return msg;
    }

    std::string::size_type pos = 0;

    if (trimmed[0] == ':') {
        std::string::size_type space_pos = trimmed.find(' ');
        if (space_pos != std::string::npos) {
            msg.prefix = trimmed.substr(1, space_pos - 1);
            pos        = space_pos + 1;
        } else {
            return msg;
        }
    }

    while (pos < trimmed.length() && trimmed[pos] == ' ') {
        pos++;
    }

    std::string::size_type cmd_end = trimmed.find(' ', pos);
    if (cmd_end == std::string::npos) {
        msg.command = trimmed.substr(pos);
        std::transform(msg.command.begin(), msg.command.end(), msg.command.begin(), ::toupper);
        return msg;
    }

    msg.command = trimmed.substr(pos, cmd_end - pos);
    std::transform(msg.command.begin(), msg.command.end(), msg.command.begin(), ::toupper);
    pos = cmd_end + 1;

    while (pos < trimmed.length()) {
        while (pos < trimmed.length() && trimmed[pos] == ' ') {
            pos++;
        }
        if (pos >= trimmed.length()) {
            break;
        }
        if (trimmed[pos] == ':') {
            msg.trailing = trimmed.substr(pos + 1);
            break;
        }
        std::string::size_type param_end = trimmed.find(' ', pos);
        if (param_end == std::string::npos) {
            msg.params.push_back(trimmed.substr(pos));
            break;
        } else {
            msg.params.push_back(trimmed.substr(pos, param_end - pos));
            pos = param_end + 1;
        }
    }

    return msg;
}

bool Parser::isValidCommand(const std::string& command) {
    if (command.empty()) {
        return false;
    }
    // TODO ADD ALL Commands
    const std::string validCommands[] = {"PASS",  "NICK", "USER",   "QUIT", "JOIN",
                                         "KICK",  "INFO", "PING",   "PONG", "PRIVMSG",
                                         "TOPIC", "KICK", "INVITE", "MODE"};

    const int numCommands = sizeof(validCommands) / sizeof(validCommands[0]);

    for (int i = 0; i < numCommands; i++) {
        if (command == validCommands[i]) {
            return true;
        }
    }

    if (command.length() == 3) {
        for (std::string::size_type i = 0; i < command.length(); i++) {
            if (!std::isdigit(command[i])) {
                return false;
            }
        }
        return true;
    }

    return false;
}

/* VALIDATE IRC NICKNAME */
bool Parser::isValidNickname(const std::string& nickname) {
    if (nickname.empty() || nickname.length() > 30) {
        return false;
    }

    // First character must be a letter
    if (!std::isalpha(nickname[0])) {
        return false;
    }

    // Subsequent characters can be letters, digits, or special chars
    for (std::string::size_type i = 1; i < nickname.length(); i++) {
        char c = nickname[i];
        if (!std::isalnum(c) && c != '-' && c != '[' && c != ']' && c != '\\' && c != '`' &&
            c != '^' && c != '{' && c != '}') {
            return false;
        }
    }

    return true;
}

/* TRIM WHITESPACE FROM STRING */
std::string Parser::trim(const std::string& str) {
    std::string::size_type start = 0;
    std::string::size_type end   = str.length();

    // Find first non-whitespace character
    while (start < end && std::isspace(str[start])) {
        start++;
    }

    // Find last non-whitespace character
    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }

    return str.substr(start, end - start);
}

/* SPLIT STRING BY DELIMITER */
std::vector<std::string> Parser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream        ss(str);
    std::string              token;

    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}
