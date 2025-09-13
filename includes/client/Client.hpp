#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <ctime>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>

enum ClientState {
    UNREGISTERED,
    PASS_OK,
    REGISTERED
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

    Client(void);
    Client(const Client& other);
    Client& operator=(const Client& other);
    ~Client(void);

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

    void appendData(std::string other);

    void sendReply(const std::string& message);

    void sendReplySilent(const std::string& message);

    void sendWelcomeMessages(void);
};

#endif
