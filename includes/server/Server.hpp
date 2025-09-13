#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <unistd.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>
#include <vector> 
#include <string>
#include <cstdio>
#include <errno.h>
#include <csignal>
#include <string.h>
#include <ctime>
#include <sstream>
#include <map>
#include "../channel/Channel.hpp"
#include "../client/Client.hpp"
#include "../pollset/Pollset.hpp"
#include "../parser/Parser.hpp"
#include "../commands/Commands.hpp"
#include "../includes/ft_irc.hpp"

class Server {
   private:
    int                              port;
    int                              socket_fd;
    std::string                      password;
    Pollset                          pollset;
    std::map<int, Client *>          clients;
    std::map<std::string, Channel *> channels;
    static bool                      signals;
    int                              timeout_seconds;
    int                              pong_timeout;
    size_t                           max_clients;

   public:

    Server(void);
    Server(int port, const std::string &password);
    Server(const Server &other);
    Server &operator=(const Server &other);
    ~Server(void);
 
    int                            getPortNumber(void) const;
    std::string                    getServerPassword(void) const;
    Client                        *getClientByFd(int fd_to_find);
    Client                        *getClientByNick(const std::string &nick);
    const std::map<int, Client *> &getClients(void) const;
    int                            getPongTimeout(void) const;
    size_t                         getPollsetIdxByFd(int fd);
    int                            getMaxClients(void) const;

    void createSocket(void);

    void initServer(void);

    void monitorConnections(void);

    void setNonBlocking(int socket);

    void connectClient(void);

    void receiveData(size_t &index);

    void disconnectClient(int client_fd);

    void throwSystemError(const std::string &msg);

    static void signalHandler(int sig);

    void handleInactiveClients(void);
    void sendPing(Client *client, time_t now);
    bool isClientTimedOut(Client *client, time_t now);
    bool isPongTimeout(Client *client, time_t now);
    void removeTimedOutClient(Client *client);

    void handleClientMessage(Client &client, const std::string &msg);

    void                              removeChannel(const std::string &channel_name);
    bool                              hasChannel(const std::string &channel_name);
    std::map<std::string, Channel *> &get_channels();
    void                              setChannel(Channel *new_channel);
};

#endif
