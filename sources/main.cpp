#include "../includes/server/Server.hpp"

#include <sstream>

bool validateArgs(char *arg1, char *arg2, int &port, int &password) {

    // validate port from 1024 to 65535
    std::stringstream ss(arg1);
    if (!(ss >> port) || !(ss >> std::ws).eof()) {
        std::cerr << "Error! Invalid port number: " << arg1 << std::endl;
        return false;
    }
    if (port < 1024 || port > 65535) {
        std::cerr << "Error! Port number is out of range of available ports: " << arg1 << std::endl;
        return false;
    }

    // validate password
}

int main(int ac, char **av) {

    if (ac != 3) {
        std::cerr << "Error! Wrong number of arguments" << std::endl;
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    int port, password;
    if (!validateArgs(av[1], av[2], port, password)) {
        std::cerr << "Error! invalid arguments" << std::endl;
        return 1;
    }
    return 0;
}