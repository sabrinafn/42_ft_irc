/*
 * Client.hpp
 *
 * Defines the Client class, representing a single IRC client connection,
 * including its socket file descriptor, input buffer, and connection state.
 */
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <ctime>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>

enum ClientState {
    UNREGISTERED, // Just connected, no authentication
    PASS_OK,      // Password accepted
    REGISTERED    // Fully authenticated (PASS + NICK + USER)
};

class Client {
   private:
    int         fd;
    std::string buffer;
    time_t      last_activity;
    bool        ping_sent;
    time_t      last_ping_sent;
    ClientState state;
    std::string ip_address;
    std::string nickname;
    std::string username;
    std::string realname;

   public:
    /* CONSTRUCTOR */
    Client(void);

    /* COPY CONSTRUCTOR */
    Client(const Client& other);

    /* OPERATORS */
    Client& operator=(const Client& other);

    /* DESTRUCTOR */
    ~Client(void);

    /* SETTERS */
    void setFd(int other);
    void setData(std::string other);
    void setLastActivity(time_t t);
    void setPingSent(bool set);
    void setLastPingSent(time_t time);
    void setState(ClientState state);
    void setIpAddress(const std::string& ip_addr);
    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);
    void setRealname(const std::string& user);

    /* GETTERS */
    int                getFd(void) const;
    std::string        getData(void) const;
    time_t             getLastActivity() const;
    bool               pingSent(void);
    time_t             getLastPingSent(void);
    ClientState        getState() const;
    const std::string& getIpAddress() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getRealname() const;
    std::string        getPrefix() const;

    /* APPEND MORE DATA TO CURRENT BUFFER DATA */
    void appendData(std::string other);

    /* SEND IRC REPLY MESSAGE TO EACH CLIENT */
    void sendReply(const std::string& message);

    /* SEND IRC REPLY WITHOUT LOGGING (used for silent bot messages) */
    void sendReplySilent(const std::string& message);

    /* SEND WELCOME MESSAGES AFTER REGISTRATION */
    void sendWelcomeMessages(void);
};

#endif
