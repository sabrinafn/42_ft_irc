#include "../includes/server/Server.hpp"
#include "../includes/utils/Utils.hpp"

int main(int ac, char **av) {

    if (ac != 3) {
        std::cerr << "Error! Wrong number of arguments" << std::endl;
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    Server server;
    try {
        setupSignals(Server::signalHandler);
        std::pair<int, std::string> pair = parsePortAndPassword(av);
        server = Server(pair.first, pair.second);
        server.initServer();
    } catch (const std::exception& e) {
        std::cerr << "Error! " << e.what() << std::endl;
    }
    server.clearServer();
    return 0;
}
