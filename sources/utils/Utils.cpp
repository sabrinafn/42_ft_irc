#include "../includes/ft_irc.hpp"

std::pair<int, std::string> parsePortAndPassword(char** av) {
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
        (password.length() < 3 || password.length() > 10)) {
        throw std::invalid_argument(
            "Password must be 3-10 characters long and contain no spaces.");
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
        if (!item.empty())
            tokens.push_back(item);
    }
    return tokens;
}

std::string getStartupTime() {
    std::time_t t       = std::time(NULL);
    std::tm*    tm_info = std::localtime(&t);

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(3) << (tm_info->tm_year + 1900) << "-" << std::setw(2)
       << (tm_info->tm_mon + 1) << "-" << std::setw(2) << tm_info->tm_mday << " "
       << std::setw(2) << tm_info->tm_hour << ":" << std::setw(2) << tm_info->tm_min << ":"
       << std::setw(2) << tm_info->tm_sec;
    return ss.str();
}
