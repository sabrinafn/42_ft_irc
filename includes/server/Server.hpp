
/*
 * Server.hpp
 *
 * Defines the Server class, responsible for managing the IRC server's socket,
 * event loop, client connections, and high-level server orchestration.
 */
#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>     // std::cout, std::cerr
#include <unistd.h>     // read, close
#include <arpa/inet.h>  // htons, inet_pton
#include <sys/socket.h> // socket, bind, listen, accept
#include <fcntl.h>      // fcntl
#include <poll.h>       // poll
#include <vector>       // std::vector
#include <string>       // std::string
#include <cstdio>       // perror
#include <errno.h>      // errno
#include <csignal>      // signal
#include <string.h>     //strerror
#include <ctime>        // std::time
#include <sstream>      // stringstream
#include <map>
#include "../channel/Channel.hpp" // channel class
#include "../client/Client.hpp"   // client class
#include "../pollset/Pollset.hpp" // pollset class
#include "../parser/Parser.hpp"   // parser class
#include "../commands/Commands.hpp"
#include "../standardReplies/StdReplies.hpp" // standard irc reply messages
#include "../includes/ft_irc.hpp"

class Server {
   private:
    int                              port;
    int                              socket_fd;
    std::string                      password;
    Pollset                          pollset;
    std::vector<Client *>            clients;
    std::map<std::string, Channel *> channels;
    static bool                      signals;
    int                              timeout_seconds;
    int                              pong_timeout;

   public:
    /* CONSTRUCTOR */
    Server(void);
    Server(int port, const std::string &password);

    /* COPY CONSTRUCTOR */
    Server(const Server &other);

    /* OPERATORS */
    Server &operator=(const Server &other);

    /* DESTRUCTOR */
    ~Server(void);

    /* SETTERS */
    void setChannel(Channel *new_channel);

    /* GETTERS */
    int                               getPortNumber(void) const;
    std::string                       getServerPassword(void) const;
    Client                           *getClientByFd(int fd_to_find);
    const std::vector<Client *>      &getClients() const;
    std::map<std::string, Channel *> &get_channels();
    int                               getPongTimeout(void) const;
    bool                              hasChannel(const std::string &channel_name);
    Client                           *getClientByNick(const std::string &nick);
    size_t                            getPollsetIdxByFd(int fd);

    /* CREATE SOCKET */
    void createSocket(void);

    /* INIT SERVER */
    void initServer(void);

    /* MONITORING FOR ACTIVITY ON FDS */
    void monitorConnections(void);

    /* SET SOCKETS AS NON BLOCKING */
    void setNonBlocking(int socket);

    /* ACCEPT A NEW CLIENT */
    void connectClient(void);

    /* RECEIVE DATA FROM REGISTERED CLIENT */
    void receiveData(size_t &index);

    /* CLEAR RESOURCES */
    void clearServer(void);

    /* DISCONNECT CLIENT */
    void disconnectClient(size_t index);

    /* THROW + SYSTEM ERROR MESSAGE */
    void throwSystemError(const std::string &msg);

    /* SIGNAL HANDLER FUNCTION */
    static void signalHandler(int sig);

    /* VERIFY CLIENTS ACTIVE TIME */
    void handleInactiveClients(void);
    void sendPing(Client* client, time_t now);
    bool isClientTimedOut(Client* client, time_t now);
    bool isPongTimeout(Client* client, time_t now);

    /* HANDLER FOR MESSAGE */
    void handleClientMessage(Client &client, const std::string &msg);

    void removeChannel(const std::string &channel_name);
};

#endif
