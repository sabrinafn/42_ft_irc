#include "../includes/ft_irc.hpp"

/* HANDLETIME */
void Commands::handleTime(Client &client, Server &server, const IRCMessage &msg) {
    (void)server;
    (void)msg;
    
    logInfo("Received !TIME command from " + client.getNickname());

    // Get current time
    time_t now = time(0);
    char timeBuffer[64];
    ctime_r(&now, timeBuffer);
    
    // Remove the newline character at the end
    std::string timeString(timeBuffer);
    if (!timeString.empty() && timeString[timeString.length()-1] == '\n') {
        timeString.erase(timeString.length()-1);
    }

    // Send the time as a PRIVMSG to the client
    std::string response = RPL_PRIVMSG(SERVER2, client.getNickname(), "Current time: " + timeString);
    client.sendReply(response);

    std::stringstream ss;
    ss << "Sent time to client [" << client.getFd() << "]: " << timeString;
    logInfo(ss.str());
}
