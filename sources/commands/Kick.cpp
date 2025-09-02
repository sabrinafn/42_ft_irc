#include "../includes/ft_irc.hpp"

/* HANDLEKICK */
// /KICK <#canal> <nick> [motivo]
void Commands::handleKick(Client& client, Server& server, const IRCMessage& msg) {
    if (msg.params.empty() || msg.params.size() < 2) {
        logError("KICK: missing parameters");
        client.sendReply(ERR_NEEDMOREPARAMS(msg.command));
        return;
    }

    std::vector<std::string> _channels = split(msg.params[0], ',');
    std::vector<std::string> targets   = split(msg.params[1], ',');

    // Motivo (se houver)
    std::string reason;
    if (msg.params.size() > 2) {
        for (size_t i = 2; i < msg.params.size(); ++i) {
            reason += msg.params[i];
            if (i != msg.params.size() - 1)
                reason += " ";
        }
    } else {
        reason = client.getNickname();
    }

    for (size_t i = 0; i < _channels.size(); ++i) {
        std::string channelName = _channels[i];
        logDebug("KICK: channel name: " + channelName);

        std::map<std::string, Channel*> all_channels = server.get_channels();

        if (!server.hasChannel(channelName)) {
            logError("KICK: channel does not exist -> " + channelName);
            client.sendReply(ERR_NOSUCHCHANNEL(channelName));
            continue;
        }

        Channel* channel = all_channels[channelName];
        if (!channel->isMember(&client)) {
            logError("KICK: client " + client.getNickname() + " is not on channel " +
                     channelName);
            client.sendReply(ERR_NOTONCHANNEL(channelName));
            continue;
        }

        if (!channel->isOperator(&client)) {
            logError("KICK: client " + client.getNickname() +
                     " tried to kick without operator privilege in " + channelName);
            client.sendReply(ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName));
            continue;
        }

        for (size_t ti = 0; ti < targets.size(); ++ti) {
            std::string targetNick = targets[ti];
            Client*     target     = server.getClientByNick(targetNick);

            if (!target) {
                logError("KICK: target user not found -> " + targetNick);
                client.sendReply(ERR_NOSUCHNICK(targetNick));
                continue;
            }

            if (!channel->isMember(target)) {
                logError("KICK: target " + targetNick + " is not in channel " + channelName);
                client.sendReply(
                    ERR_USERNOTINCHANNEL(targetNick, client.getNickname(), channelName));
                continue;
            }

            std::string kickMsg = ":" + client.getPrefix() + " KICK " + channelName + " " +
                                  targetNick + " :" + reason;

            logInfo("KICK: " + client.getNickname() + " kicked " + targetNick + " from " +
                    channelName + " (reason: " + reason + ")");

            channel->broadcast(kickMsg, &client);
            channel->removeMember(target);
            channel->removeOperator(target);

            // DEBUG APENAS: LISTAR OPERADORES
            std::vector<Client*> ops = channel->getOperators();
            std::stringstream    ssOps;
            ssOps << "Current operators in " << channelName << ":";
            for (size_t i = 0; i < ops.size(); i++) {
                ssOps << " " << ops[i]->getNickname();
            }
            logDebug(ssOps.str());

            // se nao tem operadores, adiciona primeiro membro como operador
            // ou fica sem operador se nao tiver membros
            if (ops.empty()) {
                std::vector<Client*> members = channel->getMembers();
                if (!members.empty()) {
                    channel->addOperator(members[0]);
                    logInfo("KICK: promoted " + members[0]->getNickname() +
                            " to operator in " + channelName);

                    // DEBUG APENAS: LISTAR OPERADORES DE NOVO
                    ops = channel->getOperators();
                    std::stringstream ssOps2;
                    ssOps2 << "Current operators in " << channelName << ":";
                    for (size_t i = 0; i < ops.size(); i++) {
                        ssOps2 << " " << ops[i]->getNickname();
                    }
                    logDebug(ssOps2.str());
                }
            }
        }
    }
}
