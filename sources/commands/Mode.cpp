#include "../includes/ft_irc.hpp"

void Commands::handleMode(Client &client, Server &server, const IRCMessage &msg) {
    logInfo("Received MODE command from " + client.getNickname());

    if (msg.params.empty()) {
        logError("MODE command without parameters. Sending ERR_NEEDMOREPARAMS to " +
                 client.getNickname());
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }

    std::string channelName = msg.params[0];
    logDebug("Attempting to process channel: " + channelName);

    if (!server.hasChannel(channelName)) {
        logError("Channel not found: " + channelName + ". Sending ERR_NOSUCHCHANNEL.");
        client.sendReply(ERR_NOSUCHCHANNEL(channelName));
        return;
    }

    Channel          *channel = server.get_channels()[channelName];
    std::stringstream ss1;
    ss1 << "Channel '" << channelName
        << "' found. Total members: " << channel->getMembers().size();
    logDebug(ss1.str());

    if (msg.params.size() == 1) {
        // Apenas retornar modos do canal
        logInfo("Querying modes for channel " + channelName + ". Sending RPL_CHANNELMODEIS.");
        client.sendReply(RPL_CHANNELMODEIS(channelName, channel->getModesString(),
                                           channel->getModeParameters()));

        return;
    }

    if (!channel->isOperator(&client)) {
        logError("Client " + client.getNickname() + " is not a channel operator on " +
                 channelName + ". Sending ERR_CHANOPRIVSNEEDED.");
        client.sendReply(ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName));
        return;
    }

    std::string mode = msg.params[1];
    if (mode.empty() || (mode[0] != '+' && mode[0] != '-')) {
        logError("Invalid mode: '" + mode + "'. Sending ERR_UNKNOWNMODE.");
        client.sendReply(ERR_UNKNOWNMODE(mode));
        return;
    }

    size_t argIndex = 2; // índice inicial para argumentos extras
    bool   add      = (mode[0] == '+');

    for (size_t i = 1; i < mode.size(); ++i) {
        std::string arg        = (argIndex < msg.params.size()) ? msg.params[argIndex++] : "";
        std::string modeChange = (add ? "+" : "-");
        modeChange += mode[i];

        logDebug("Processing mode: " + modeChange +
                 (arg.empty() ? "" : " with argument: " + arg));

        switch (mode[i]) {
            case 'i':
                if (add) {
                    logInfo("Adding mode +i to " + channelName);
                    channel->addMode(Channel::INVITE_ONLY);
                } else {
                    logInfo("Removing mode -i from " + channelName);
                    channel->removeMode(Channel::INVITE_ONLY);
                }
                break;
            case 't':
                if (add) {
                    logInfo("Adding mode +t to " + channelName);
                    channel->addMode(Channel::TOPIC_RESTRICTED);
                } else {
                    logInfo("Removing mode -t from " + channelName);
                    channel->removeMode(Channel::TOPIC_RESTRICTED);
                }
                break;
            case 'k':
                if (add) {
                    logInfo("Adding mode +k to " + channelName + " with key '" + arg + "'");
                    channel->addMode(Channel::KEY_REQUIRED);
                    channel->setKey(arg);
                } else {
                    logInfo("Removing mode -k from " + channelName);
                    channel->removeMode(Channel::KEY_REQUIRED);
                    channel->setKey("");
                }
                break;
            case 'l':
                if (add) {
                    logInfo("Adding mode +l to " + channelName + " with limit " + arg);
                    channel->addMode(Channel::LIMIT_SET);
                    if (!arg.empty())
                        channel->setLimit(std::atoi(arg.c_str()));
                } else {
                    logInfo("Removing mode -l from " + channelName);
                    channel->removeMode(Channel::LIMIT_SET);
                    channel->removeLimit();
                }
                break;
            case 'o':
                if (arg.empty()) {
                    logError("Mode 'o' without argument. Skipping.");
                    break;
                }
                {
                    Client *target = server.getClientByNick(arg);
                    if (!target) {
                        logError("Target client for mode 'o' not found: " + arg);
                        break;
                    }
                    if (add) {
                        logInfo("Adding operator " + arg + " on " + channelName);
                        channel->addOperator(target);
                    } else {
                        logInfo("Removing operator " + arg + " from " + channelName);
                        channel->removeOperator(target);
                    }
                }
                break;
            default:
                logError("Unknown mode: " + std::string(1, mode[i]) +
                         ". Sending ERR_UNKNOWNMODE.");
                break;
        }

        // Broadcast da alteração para todos do canal
        std::string broadcastMsg =
            ":" + client.getNickname() + " MODE " + channelName + " " + modeChange;
        if (!arg.empty())
            broadcastMsg += " " + arg;
        logDebug("Broadcasting mode change to channel: " + broadcastMsg);
        channel->broadcast(broadcastMsg, NULL);
    }
}
