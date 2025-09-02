#include "../includes/ft_irc.hpp"


void handleMode(Client &client, Server &server, const IRCMessage &msg) {
    if (msg.params.empty()) return;

    std::string channelName = msg.params[0];

    if (!server.hasChannel(channelName)) {
        client.sendReply(ERR_NOSUCHCHANNEL(channelName));
        return;
    }

    Channel *channel = server.get_channels()[channelName];

    if (msg.params.size() == 1) {
        // Apenas retornar modos do canal
       // client.sendReply(RPL_CHANNELMODEIS(channelName, channel->getModesString(), channel->getModeParams()));
        return;
    }

    if (!channel->isOperator(&client)) {
        client.sendReply(ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName));
        return;
    }

    std::string mode = msg.params[1];
    if (mode.empty() || (mode[0] != '+' && mode[0] != '-')) {
        client.sendReply(ERR_UNKNOWNMODE(mode));
        return;
    }

    size_t argIndex = 2; // índice inicial para argumentos extras
    bool add = (mode[0] == '+');

    for (size_t i = 1; i < mode.size(); ++i) {
        std::string arg = (argIndex < msg.params.size()) ? msg.params[argIndex++] : "";
        std::string modeChange = (add ? "+" : "-");
        modeChange += mode[i];

        switch (mode[i]) {
            case 'i':
                if (add) channel->addMode(Channel::INVITE_ONLY);
                else channel->removeMode(Channel::INVITE_ONLY);
                break;
            case 't':
                if (add) channel->addMode(Channel::TOPIC_RESTRICTED);
                else channel->removeMode(Channel::TOPIC_RESTRICTED);
                break;
            case 'k':
                if (add) {
                    channel->addMode(Channel::KEY_REQUIRED);
                    channel->setKey(arg);
                } else {
                    channel->removeMode(Channel::KEY_REQUIRED);
                    channel->setKey("");
                }
                break;
            case 'l':
                if (add) {
                    channel->addMode(Channel::LIMIT_SET);
                    if (!arg.empty()) channel->setLimit(std::atoi(arg.c_str()));
                } else {
                    channel->removeMode(Channel::LIMIT_SET);
                    channel->removeLimit();
                }
                break;
            case 'o':
                if (arg.empty()) break;
                {
                    Client* target = server.serverGetClientByNick(arg);
                    if (!target) break;
                    if (add) channel->addOperator(target);
                    else channel->removeOperator(target);
                }
                break;
            default:
                break;
        }

        // Broadcast da alteração para todos do canal
        std::string broadcastMsg = ":" + client.getNickname() + " MODE " + channelName + " " + modeChange;
        if (!arg.empty()) broadcastMsg += " " + arg;
        channel->broadcast(broadcastMsg, &client);
    }
}