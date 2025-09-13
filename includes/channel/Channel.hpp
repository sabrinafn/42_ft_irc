#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>
#include "../client/Client.hpp"

class Channel {
   public:
    enum ChannelMode {
        INVITE_ONLY,
        TOPIC_RESTRICTED,
        KEY_REQUIRED,
        LIMIT_SET
    };

   private:
    std::string name;
    std::string key;
    std::string topic;
    int         limit;

    std::vector<ChannelMode> modes; 
    std::vector<Client*>     members;
    std::vector<Client*>     ops;
    std::vector<Client*>     invited;

   public:

   Channel(const std::string& name);
   Channel(const Channel& other);
   Channel& operator=(const Channel& other);
   ~Channel(void);
   
   bool isEmptyChannel() const;
    
    std::string              getName() const;
    std::string              getTopic() const;
    std::string              getKey() const;
    int                      getLimit() const;
    std::vector<ChannelMode> getModes() const;
    std::string              getModesString() const;
    std::string              getModeParameters() const;
    
    void setTopic(const std::string& newTopic);
    void setKey(const std::string& newKey);
    void removeKey();
    void setLimit(int newLimit);
    void removeLimit();
    
    void addMode(ChannelMode mode);
    void removeMode(ChannelMode mode);
    bool hasMode(ChannelMode mode) const;
    
    void                 addMember(Client* client); 
    void                 removeMember(Client* client);
    bool                 isMember(Client* client) const;
    std::vector<Client*> getMembers() const;

    void                 addOperator(Client* client);
    void                 removeOperator(Client* client);
    bool                 isOperator(Client* client) const;
    std::vector<Client*> getOperators() const;
    
    void invite(Client* client);
    bool isInvited(Client* client) const;
    void removeInvite(Client* client);
    
    void broadcast(const std::string& message, Client* sender = NULL);
};

#endif
