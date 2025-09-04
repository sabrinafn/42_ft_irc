#include "../includes/ft_irc.hpp"

/* CONSTRUCTOR */
Channel::Channel(const std::string& name) : name(name), limit(0) {
}

/* COPY CONSTRUCTOR */
Channel::Channel(const Channel& other) {
    *this = other;
}

/* OPERATOR = */
Channel& Channel::operator=(const Channel& other) {
    if (this != &other) {
        this->name    = other.name;
        this->topic   = other.topic;
        this->key     = other.key;
        this->limit   = other.limit;
        this->modes   = other.modes;
        this->members = other.members;
        this->ops     = other.ops;
        this->invited = other.invited;
    }
    return *this;
}

/* DESTRUCTOR */
Channel::~Channel(void) {
}

bool Channel::isEmptyChannel() const {
    return members.empty();
}

/* GETTER FOR NAME */
std::string Channel::getName() const {
    return name;
}

/* GETTER FOR TOPIC */
std::string Channel::getTopic() const {
    return topic;
}

/* GETTER FOR KEY */
std::string Channel::getKey() const {
    return key;
}

/* GETTER FOR LIMIT */
int Channel::getLimit() const {
    return limit;
}

/* GETTER FOR MODES */
std::vector<Channel::ChannelMode> Channel::getModes() const {
    return modes;
}

/* GETTER FOR MODES STRING */
std::string Channel::getModesString() const {
    std::string modeStr = "+";
    if (hasMode(INVITE_ONLY))
        modeStr += "i";
    if (hasMode(TOPIC_RESTRICTED))
        modeStr += "t";
    if (hasMode(KEY_REQUIRED))
        modeStr += "k";
    if (hasMode(LIMIT_SET))
        modeStr += "l";
    return (modeStr.length() > 1) ? modeStr : "";
}

/* SETTER FOR TOPIC */
void Channel::setTopic(const std::string& newTopic) {
    topic = newTopic;
}

/* SETTER FOR KEY */
void Channel::setKey(const std::string& newKey) {
    key = newKey;
    addMode(KEY_REQUIRED);
}

/* REMOVE FOR KEY */
void Channel::removeKey() {
    key.clear();
    removeMode(KEY_REQUIRED);
}

/* SETTER FOR LIMIT */
void Channel::setLimit(int newLimit) {
    limit = newLimit;
    addMode(LIMIT_SET);
}

/* REMOVE FOR LIMIT */
void Channel::removeLimit() {
    limit = 0;
    removeMode(LIMIT_SET);
}

// Gerenciamento de modos

/* ADD MODE  - Ativa um modo (ex: +i) */
void Channel::addMode(ChannelMode mode) {
    if (!hasMode(mode))
        modes.push_back(mode);
}

/* REMOVE MODE - Desativa um modo (ex: -i)*/
void Channel::removeMode(ChannelMode mode) {
    std::vector<ChannelMode>::iterator it = std::find(modes.begin(), modes.end(), mode);
    if (it != modes.end())
        modes.erase(it);
}

/* FIND MODE - Verifica se um modo está ativo */
bool Channel::hasMode(ChannelMode mode) const {
    return std::find(modes.begin(), modes.end(), mode) != modes.end();
}

// Membros

/* ADD MEMBER */
void Channel::addMember(Client* client) {
    if (!isMember(client))
        members.push_back(client);
}

/* REMOVE MEMBER */
void Channel::removeMember(Client* client) {
    members.erase(std::remove(members.begin(), members.end(), client), members.end());
    ops.erase(std::remove(ops.begin(), ops.end(), client), ops.end());
    invited.erase(std::remove(invited.begin(), invited.end(), client), invited.end());
}

/* FIND MEMBER */
bool Channel::isMember(Client* client) const {
    return std::find(members.begin(), members.end(), client) != members.end();
}

/* GET ALL MEMBERS */
std::vector<Client*> Channel::getMembers() const {
    return members;
}

// Operadores de canal

/* ADD OPERATOR */
void Channel::addOperator(Client* client) {
    if (isMember(client) && !isOperator(client))
        ops.push_back(client);
}

/* REMOVE OPERATOR */
void Channel::removeOperator(Client* client) {
    ops.erase(std::remove(ops.begin(), ops.end(), client), ops.end());
}

/* FIND OPERATOR */
bool Channel::isOperator(Client* client) const {
    return std::find(ops.begin(), ops.end(), client) != ops.end();
}

/* GETTER FOR OPERATORS */
std::vector<Client*> Channel::getOperators() const {
    return ops;
}

// Convites

/* INVITE A CLIENT */
void Channel::invite(Client* client) {
    if (!isInvited(client))
        invited.push_back(client);
}

/* CHECK IF CLIENT WAS INVITED */
bool Channel::isInvited(Client* client) const {
    return std::find(invited.begin(), invited.end(), client) != invited.end();
}

/* REMOVE A CLIENT */
void Channel::removeInvite(Client* client) {
    invited.erase(std::remove(invited.begin(), invited.end(), client), invited.end());
}

// Broadcast
// Envio de mensagens para todos os membros do canal
void Channel::broadcast(const std::string& message, Client* client) {
    std::stringstream ss;
    ss << "Broadcast message sent to " << members.size()
       << " members, client: " << (client ? client->getNickname() : "NULL");
    logDebug(ss.str());

    for (size_t i = 0; i < members.size(); ++i) {
        std::stringstream ss;
        ss << "Channel member [" << i << "] = " << members[i]->getNickname();
        logDebug(ss.str());
        if (client == NULL || members[i] != client) {
            members[i]->sendReply(message);
        }
    }
}
