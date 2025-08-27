#include "../includes/ft_irc.hpp"

/* HANDLEQUIT */
void Commands::handleQuit(Client &client, Server &server, const IRCMessage &msg) {
    std::string quitMessage = msg.trailing.empty() ? "Client Quit" : msg.trailing;

    // Send quit message to client (optional)
    std::cout << "Client [" << client.getFd() << "] (" << client.getNickname()
              << ") quit: " << quitMessage << std::endl;

    // Find the client in pollset and disconnect
    size_t pollIndex = server.getPollsetIdxByFd(client.getFd());
    if (pollIndex != static_cast<size_t>(-1)) {
        server.disconnectClient(pollIndex);
    }
}
