#include "../includes/ft_irc.hpp"

/* HANDLEJOKE */
void Commands::handleJoke(Client &client, Server &server, const IRCMessage &msg) {
    (void)server;
    (void)msg;
    
    logInfo("Received !JOKE command from " + client.getNickname());

    // Array of jokes (static to avoid recreation)
    static std::vector<std::string> jokes;
    if (jokes.empty()) {
        jokes.push_back("Why don't scientists trust atoms? Because they make up everything!");
        jokes.push_back("Why did the scarecrow win an award? He was outstanding in his field!");
        jokes.push_back("Why don't eggs tell jokes? They'd crack each other up!");
        jokes.push_back("What do you call a fake noodle? An impasta!");
        jokes.push_back("Why did the math book look so sad? Because it had too many problems!");
        jokes.push_back("What do you call a bear with no teeth? A gummy bear!");
        jokes.push_back("Why don't skeletons fight each other? They don't have the guts!");
        jokes.push_back("What do you call a fish wearing a bowtie? So-fish-ticated!");
        jokes.push_back("Why did the coffee file a police report? It got mugged!");
        jokes.push_back("What do you call a dinosaur that crashes his car? Tyrannosaurus Wrecks!");
    }

    // Get a random joke
    static bool seeded = false;
    if (!seeded) {
        srand(time(NULL));
        seeded = true;
    }
    int randomIndex = rand() % jokes.size();
    std::string joke = jokes[randomIndex];

    // Send the joke as a PRIVMSG to the client
    std::string response = RPL_PRIVMSG(SERVER2, client.getNickname(), joke);
    client.sendReply(response);

    std::stringstream ss;
    ss << "Sent joke to client [" << client.getFd() << "]: " << joke;
    logInfo(ss.str());
}
