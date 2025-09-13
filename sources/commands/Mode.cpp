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
        if (!channel->isOperator(&client)) {
            if (channel->getLimit() == 0) {
                client.sendReply(RPL_CHANNELMODEIS(channelName, channel->getModesString(), ""));
            } else {
                std::stringstream ss;
                ss << channel->getLimit();
                client.sendReply(RPL_CHANNELMODEIS(channelName, channel->getModesString(), ss.str()));
            }

        }
        else {
            client.sendReply(RPL_CHANNELMODEIS(channelName, channel->getModesString(),
                                           channel->getModeParameters()));
        }
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
        client.sendReply(ERR_UNKNOWNMODE(client.getNickname(), mode));
        return;
    }

    bool   modeApplied = false;
    size_t argIndex    = 2;
    bool   add         = (mode[0] == '+');

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
                modeApplied = true;
                break;
            case 't':
                if (add) {
                    logInfo("Adding mode +t to " + channelName);
                    channel->addMode(Channel::TOPIC_RESTRICTED);
                } else {
                    logInfo("Removing mode -t from " + channelName);
                    channel->removeMode(Channel::TOPIC_RESTRICTED);
                }
                modeApplied = true;
                break;
            case 'k':
                if (add) {
                    if (arg.empty()) {
                        logError("Mode +k requires a key. Sending ERR_NEEDMOREPARAMS.");
                        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
                        break;
                    } else if (channel->hasMode(Channel::KEY_REQUIRED) &&
                               !channel->getKey().empty()) {
                        logError("Channel " + channelName +
                                 " already has a key. Sending ERR_KEYSET.");
                        client.sendReply(ERR_KEYSET(client.getNickname(), channelName));
                        break;
                    }
                    logInfo("Adding mode +k to " + channelName + " with key '" + arg + "'");
                    channel->addMode(Channel::KEY_REQUIRED);
                    channel->setKey(arg);
                    modeApplied = true;
                } else {
                    logInfo("Removing mode -k from " + channelName);
                    channel->removeMode(Channel::KEY_REQUIRED);
                    channel->removeKey();
                    modeApplied = true;
                }
                break;
            case 'l':
                if (add) {
                    if (arg.empty() || !isStrDigit(arg)) {
                        logError(
                            "Mode +l requires a numeric limit. Sending ERR_NEEDMOREPARAMS.");
                        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
                        break;
                    }
                    logInfo("Adding mode +l to " + channelName + " with limit " + arg);
                    channel->addMode(Channel::LIMIT_SET);
                    channel->setLimit(std::atoi(arg.c_str()));
                    modeApplied = true;
                } else {
                    logInfo("Removing mode -l from " + channelName);
                    channel->removeMode(Channel::LIMIT_SET);
                    channel->removeLimit();
                    modeApplied = true;
                }
                break;
            case 'o': {
                if (arg.empty()) {
                    logError("Mode +o requires a nickname. Sending ERR_NEEDMOREPARAMS.");
                    client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
                    break;
                }
                Client *target = server.getClientByNick(arg);
                if (!target) {
                    logError("Target client for mode 'o' not found: " + arg +
                             ". Sending ERR_NOSUCHNICK.");
                    client.sendReply(ERR_NOSUCHNICK(arg));
                    break;
                }
                if (add) {
                    if (!channel->isOperator(target)) { 
                        channel->addOperator(target);
                        modeApplied = true;
                    }
                } else {
                    if (channel->isOperator(target)) { 
                        channel->removeOperator(target);
                        modeApplied = true;
                    }
                }
                break;
            }
            default:
                logError("Unknown mode: " + std::string(1, mode[i]) +
                         ". Sending ERR_UNKNOWNMODE.");
                client.sendReply(
                    ERR_UNKNOWNMODE(client.getNickname(), std::string(1, mode[i])));
                break;
        }


        if (modeApplied) {
            std::string broadcastMsg =
                ":" + client.getNickname() + " MODE " + channelName + " " + modeChange;
            if (!arg.empty())
                broadcastMsg += " " + arg;
            logDebug("Broadcasting mode change to channel: " + broadcastMsg);
            channel->broadcast(broadcastMsg, NULL);
        }
    }
}
