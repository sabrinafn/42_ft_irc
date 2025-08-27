#include "../includes/ft_irc.hpp"

std::pair<int, std::string> parsePortAndPassword(char **av) {
    // validate port from 1024 to 65535
    int               port;
    std::stringstream ss(av[1]);
    if (!(ss >> port) || !(ss >> std::ws).eof()) {
        throw std::invalid_argument("Invalid port number: '" + std::string(av[1]) + "'");
    }
    if (port < 1024 || port > 65535) {
        throw std::invalid_argument("Port number is out of range of available ports: '" +
                                    std::string(av[1]) + "'");
    }

    // validate password
    std::string password = av[2];
    if (password.find(" ") != std::string::npos ||
        (password.length() < 8 || password.length() > 16)) {
        throw std::invalid_argument(
            "Password must be 8-16 characters long and contain no spaces.");
    }

    std::pair<int, std::string> pair;
    pair.first  = port;
    pair.second = password;
    return pair;
}

void setupSignals(void (*handler)(int)) {
    // ctrl+c
    signal(SIGINT, handler);
    // ctrl+'\'
    signal(SIGQUIT, handler);
    // kill [pid]
    signal(SIGTERM, handler);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream        ss(str);
    std::string              item;
    while (std::getline(ss, item, delimiter)) {
        if (!item.empty()) tokens.push_back(item);
    }
    return tokens;
}
