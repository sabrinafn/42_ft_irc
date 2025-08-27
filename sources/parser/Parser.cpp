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
        // RFC 1459: message length is 512!!! including (\r\n)
        if (line.size() > 510) {
            line = line.substr(0, 510);
        }
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
    if (command.empty()) return false;

    // Known, supported command tokens (keep in sync with Commands map)
    static const char* kKnown[] = {"PASS",   "NICK",   "USER",   "QUIT",  "JOIN",
                                   "KICK",   "PING",   "PONG",   "PRIVMSG","TOPIC",
                                   "INVITE", "MODE"};
    for (size_t i = 0; i < sizeof(kKnown) / sizeof(kKnown[0]); ++i) {
        if (command == kKnown[i]) return true;
    }

    // Accept 3-digit numeric replies
    if (command.length() == 3 && std::isdigit(static_cast<unsigned char>(command[0])) &&
        std::isdigit(static_cast<unsigned char>(command[1])) &&
        std::isdigit(static_cast<unsigned char>(command[2]))) {
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
    while (start < end && std::isspace(static_cast<unsigned char>(str[start]))) {
        start++;
    }

    // Find last non-whitespace character
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        end--;
    }

    return str.substr(start, end - start);
}

/* Removed duplicate split helper; use global split() in Utils instead. */
