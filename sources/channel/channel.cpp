#include "channel.hpp"

Channel::Channel(const std::string& name) : name(name), limit(0), modes(0) {}

// Getters
std::string Channel::getName() const {
    return name;
}

std::string Channel::getTopic() const {
    return topic;
}

std::string Channel::getKey() const {
    return key;
}

int Channel::getLimit() const {
    return limit;
}

int Channel::getModes() const {
    return modes;
}

std::string Channel::getModesString() const {
    std::string modeStr = "+";
    if (modes & INVITE_ONLY) modeStr += "i";
    if (modes & TOPIC_RESTRICTED) modeStr += "t";
    if (modes & KEY_REQUIRED) modeStr += "k";
    if (modes & LIMIT_SET) modeStr += "l";
    return (modeStr.length() > 1) ? modeStr : "";
}

// Setters
void Channel::setTopic(const std::string& newTopic) {
    topic = newTopic;
}

void Channel::setKey(const std::string& newKey) {
    key = newKey;
    addMode(KEY_REQUIRED);
}

void Channel::removeKey() {
    key.clear();
    removeMode(KEY_REQUIRED);
}

void Channel::setLimit(int newLimit) {
    limit = newLimit;
    addMode(LIMIT_SET);
}

void Channel::removeLimit() {
    limit = 0;
    removeMode(LIMIT_SET);
}

// Gerenciamento de modos
void Channel::addMode(ChannelMode mode) {
    modes |= mode;
}

void Channel::removeMode(ChannelMode mode) {
    modes &= ~mode;
}

bool Channel::hasMode(ChannelMode mode) const {
    return modes & mode;
}

// Membros
void Channel::addMember(Client* client) {
    members.insert(client);
}

void Channel::removeMember(Client* client) {
    members.erase(client);
    ops.erase(client);
    invited.erase(client);
}

bool Channel::isMember(Client* client) const {
    return members.find(client) != members.end();
}

std::set<Client*> Channel::getMembers() const {
    return members;
}

// Operadores
void Channel::addOperator(Client* client) {
    if (isMember(client))
        ops.insert(client);
}

void Channel::removeOperator(Client* client) {
    ops.erase(client);
}

bool Channel::isOperator(Client* client) const {
    return ops.find(client) != ops.end();
}

// Convites
void Channel::invite(Client* client) {
    invited.insert(client);
}

bool Channel::isInvited(Client* client) const {
    return invited.find(client) != invited.end();
}

void Channel::removeInvite(Client* client) {
    invited.erase(client);
}

// Broadcast
void Channel::broadcast(const std::string& message, Client* sender) {
    for (std::set<Client*>::iterator it = members.begin(); it != members.end(); ++it) {
        if (*it != sender) {
            // Aqui você enviaria a mensagem ao cliente via socket, por exemplo:
            // (*it)->sendMessage(message); — você precisa implementar esse método em Client
        }
    }
}