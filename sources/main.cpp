#include "../includes/server/Server.hpp"

#include <sstream>

bool validatePortAndPassword(char **av, Server &server) {

    // validate port from 1024 to 65535
    int port;
    std::stringstream ss(av[1]);
    if (!(ss >> port) || !(ss >> std::ws).eof()) {
        std::cerr << "Error! Invalid port number: '" << av[1] << "'" << std::endl;
        return false;
    }
    if (port < 1024 || port > 65535) {
        std::cerr << "Error! Port number is out of range of available ports: '" << av[1] << "'" << std::endl;
        return false;
    }

    // validate password
    std::string password = av[2];

    if (password.find(" ") != std::string::npos || (password.length() < 8 || password.length() > 16)) {
        std::cerr << "Error: Password must be 8-16 characters long and contain no spaces." << std::endl;
        return false;
    }

    server.setPortNumber(port);
    server.setServerPassword(password);
    return true;
}

int main(int ac, char **av) {

    if (ac != 3) {
        std::cerr << "Error! Wrong number of arguments" << std::endl;
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    Server server;
    if (!validatePortAndPassword(av, server))
        return 1;
    std::cout << "Server starting on port " << server.getPortNumber()
          << " with password '" << server.getServerPassword() << "'" << std::endl;

    server.initServer();
    return 0;
}
