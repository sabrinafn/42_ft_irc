//#include "server/Server.hpp"
//#include "client/Client.hpp"
#include <iostream>

int main (int ac, char **av) {

    (void)av;
    if (ac != 3) {
        std::cerr << "Error: invalid input." << std::endl;
        std::cerr << "Program usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    return 0;
}