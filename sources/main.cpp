#include "../includes/server/Server.hpp"

#include <sstream>
#include <utility>

std::pair<int, std::string> validatePortAndPassword(char **av) {

    // validate port from 1024 to 65535
    int port;
    std::stringstream ss(av[1]);
    if (!(ss >> port) || !(ss >> std::ws).eof()) {
        throw std::invalid_argument("Error! Invalid port number: '" + std::string(av[1]) + "'");
    }
    if (port < 1024 || port > 65535) {
        throw std::invalid_argument("Error! Port number is out of range of available ports: '" + std::string(av[1]) + "'");
    }

    // validate password
    std::string password = av[2];

    if (password.find(" ") != std::string::npos || (password.length() < 8 || password.length() > 16)) {
        throw std::invalid_argument("Error: Password must be 8-16 characters long and contain no spaces.");
    }

    std::pair<int, std::string> pair;
    pair.first = port;
    pair.second = password;
    return pair;
}

int main(int ac, char **av) {

    if (ac != 3) {
        std::cerr << "Error! Wrong number of arguments" << std::endl;
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    try {
        Server server;
        std::pair<int, std::string> pair = validatePortAndPassword(av);
        server.setPortNumber(pair.first);
        server.setServerPassword(pair.second);
        std::cout << "Server starting on port " << server.getPortNumber()
              << " with password '" << server.getServerPassword() << "'" << std::endl;

        server.initServer();
    } catch (const std::exception& e) {
        std::cerr << "Error! " << e.what() << std::endl;
    }

    return 0;
}
