
/*
 * Server.hpp
 *
 * Defines the Server class, responsible for managing the IRC server's socket,
 * event loop, client connections, and high-level server orchestration.
 */
#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream> // std::cout, std::cerr
#include <unistd.h> // read, close
#include <arpa/inet.h> // htons, inet_pton
#include <sys/socket.h> // socket, bind, listen, accept
#include <fcntl.h> // fcntl
#include <poll.h> // poll
#include <vector> // std::vector
#include <string> // std::string
#include <cstdio> // perror
#include <errno.h> // errno
#include <csignal> // signal
#include <string.h> //strerror
#include <ctime> // std::time
#include <sstream> // stringstream
#include <map>
#include "../channel/Channel.hpp"

#include "../client/Client.hpp" // client class
#include "../pollset/Pollset.hpp" // pollset class
#include "../parser/Parser.hpp" // parser class

class Server {

    private:
        int port;
        int socket_fd;
        std::string password;
        std::vector<Client> clients;
        std::map<std::string, Channel*> channels;
        Pollset pollset;
        static bool signals;
        int timeout_seconds;
        int pong_timeout;

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
        //void setPortNumber(int other);
        //void setServerPassword(std::string other);

        /* GETTERS */
        int getPortNumber(void) const;
        std::string getServerPassword(void) const;
        Client *getClientByFd(int fd_to_find);
        std::map<std::string, Channel*> &get_channels();

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

        /* FIND FD INDEX IN POLLSET BY FD IN CLIENT */
        size_t getPollsetIdxByFd(int fd);

        /* VERIFY CLIENTS ACTIVE TIME */
        void handleInactiveClients(void);

        /* HANDLER FOR MESSAGE */
        /* only created now to deal with PONG */
        void handleClientMessage(Client &client, const std::string &msg);

        /* PROCESS IRC COMMANDS */
        void processIRCMessage(Client &client, const IRCMessage &msg);

        /* IRC COMMAND HANDLERS */
        void handlePass(Client &client, const IRCMessage &msg);
        void handleNick(Client &client, const IRCMessage &msg);
        void handleUser(Client &client, const IRCMessage &msg);
        void handlePing(Client &client, const IRCMessage &msg);
        void handlePong(Client &client, const IRCMessage &msg);
        void handleQuit(Client &client, const IRCMessage &msg);
        void handleJoin(Client &client, const IRCMessage &msg);
        void handlePrivmsg(Client &client, const IRCMessage &msg);
        /* SEND IRC REPLY TO CLIENT */
        void sendReply(int fd, int code, const std::string& nickname, const std::string& message);
        void sendRawMessage(int fd, const std::string& message);

        /* SEND WELCOME MESSAGES AFTER REGISTRATION */
        void sendWelcomeMessages(Client &client);

        /* CHECK IF NICKNAME IS ALREADY IN USE */
        bool isNicknameInUse(const std::string& nickname, int excludeFd = -1);

        /* CHECK IF  IS VALID CHANNEL NAME */
        bool Server::isValidChannelName(const std::string& name);

        /* CHECK IF  IS VALID KEY */
        bool Server::isValidkey(std::string key);
      
        /*ADD CHANNEL*/
        void addChannel(Channel* new_channel);
        /* find channel */
        bool channelnotExists(const std::string& channel_name);
    };

#endif
