#include "../includes/ft_irc.hpp"

Channel::Channel(const std::string& name) : name(name), limit(0) {
}

Channel::Channel(const Channel& other) {
    *this = other;
}

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

Channel::~Channel(void) {
}

bool Channel::isEmptyChannel() const {
    return members.empty();
}

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

std::vector<Channel::ChannelMode> Channel::getModes() const {
    return modes;
}

std::string Channel::getModeParameters() const {
    std::stringstream ss;
    bool              has_prev_param = false;

    if (!this->key.empty()) {
        ss << this->key;
        has_prev_param = true;
    }

    if (this->limit > 0) {
        if (has_prev_param) {
            ss << " ";
        }
        ss << this->limit;
    }
    return ss.str();
}

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


void Channel::addMode(ChannelMode mode) {
    if (!hasMode(mode))
        modes.push_back(mode);
}

void Channel::removeMode(ChannelMode mode) {
    std::vector<ChannelMode>::iterator it = std::find(modes.begin(), modes.end(), mode);
    if (it != modes.end())
        modes.erase(it);
}


bool Channel::hasMode(ChannelMode mode) const {
    return std::find(modes.begin(), modes.end(), mode) != modes.end();
}


void Channel::addMember(Client* client) {
    if (!isMember(client))
        members.push_back(client);
}

void Channel::removeMember(Client* client) {
    members.erase(std::remove(members.begin(), members.end(), client), members.end());
    ops.erase(std::remove(ops.begin(), ops.end(), client), ops.end());
    invited.erase(std::remove(invited.begin(), invited.end(), client), invited.end());
}

bool Channel::isMember(Client* client) const {
    return std::find(members.begin(), members.end(), client) != members.end();
}

std::vector<Client*> Channel::getMembers() const {
    return members;
}




void Channel::addOperator(Client* client) {
    if (isMember(client) && !isOperator(client))
        ops.push_back(client);
}


void Channel::removeOperator(Client* client) {
    ops.erase(std::remove(ops.begin(), ops.end(), client), ops.end());
}

bool Channel::isOperator(Client* client) const {
    return std::find(ops.begin(), ops.end(), client) != ops.end();
}

std::vector<Client*> Channel::getOperators() const {
    return ops;
}


void Channel::invite(Client* client) {
    if (!isInvited(client))
        invited.push_back(client);
}

bool Channel::isInvited(Client* client) const {
    return std::find(invited.begin(), invited.end(), client) != invited.end();
}

void Channel::removeInvite(Client* client) {
    invited.erase(std::remove(invited.begin(), invited.end(), client), invited.end());
}

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
